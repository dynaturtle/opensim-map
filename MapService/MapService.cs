using System;
using System.IO;
using System.Collections.Generic;
using System.Reflection;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Threading;
using log4net;
using OpenSim.Framework;
using OpenSim.Framework.Servers.HttpServer;
using OpenSim.Region.Framework.Scenes;
using OpenSim.Region.Physics.Meshing;
using Nini.Config;

namespace OpenSim.ApplicationPlugins.MapService
{
    class MapService:IApplicationPlugin
    {
        #region members
        private OpenSimBase _app; // The 'server'
        private BaseHttpServer _httpd; // The server's RPC interface
        protected static readonly ILog m_log =
           LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);

        /// <summary>
        /// Name of the plugin
        /// </summary>
        private string m_name = "MapService";

        // TODO: required by IPlugin, but likely not at all right
        private string m_version = "0.1";

        private IConfigSource m_config;
        private ObjectLayer m_objectLayer;
        private TerrainLayer m_terrainLayer;
        private AgentLayer m_agentLayer;
        private Dictionary<string, BaseLayer> layerDict;
        /// <summary>
        /// OpenSimMain application
        /// </summary>
        public OpenSimBase App
        {
            get { return _app; }
        }

        /// <summary>
        /// RPC server
        /// </summary>
        public BaseHttpServer HttpServer
        {
            get { return _httpd; }
        }

        public string Name { get { return m_name; } }

        public string Version
        {
            get { return m_version; }
        }
        #endregion

        Bitmap WMSGetMap(BBOXF bbox, int height, int width, String[] layers)
        {
            Bitmap compositeBMP = new Bitmap(width, height);
            Graphics gf = Graphics.FromImage(compositeBMP);
            for (int i = 0; i < layers.Length; i++)
            {
                Bitmap layerBatch = layerDict[layers[i]].GetRegionByBitmap(bbox, new Size(width, height));
                gf.CompositingQuality = CompositingQuality.GammaCorrected;
                gf.CompositingMode = CompositingMode.SourceOver;
                gf.DrawImage(layerBatch, new Rectangle(0,0,height,width));
                layerBatch.Dispose();
            }
            return compositeBMP;
        }

        public string owsHandler(string request, string path, string param,
                                      OSHttpRequest httpRequest, OSHttpResponse httpResponse)
        {
            switch (httpRequest.QueryString["SERVICE"])
            {
                case "WMS":
                    if (httpRequest.QueryString["REQUEST"] == "GetMap")
                    {
                        httpResponse.ContentType = "image/png";

                        //parse query string
                        string[] layers = httpRequest.QueryString["LAYERS"].Split(',');
                        BBOXF bbox = new BBOXF(httpRequest.QueryString["BBOX"]);
                        int height = Int32.Parse(httpRequest.QueryString["HEIGHT"]);
                        int width = Int32.Parse(httpRequest.QueryString["WIDTH"]);
                        string format = httpRequest.QueryString["FORMAT"];

                        //get queryImg
                        Bitmap queryImg = WMSGetMap(bbox, height, width, layers);
                        //convert bitmap to base64 string
                        System.IO.MemoryStream stream = new System.IO.MemoryStream();
                        queryImg.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
                        queryImg.Dispose();
                        byte[] byteImage = stream.ToArray();
                        stream.Dispose();
                        return Convert.ToBase64String(byteImage);
                    }
                    else if (httpRequest.QueryString["REQUEST"] == "GetCapabilities")
                    {
                        httpResponse.ContentType = "text/xml";
                        string capDes = "";
                        TextReader textReader = new StreamReader("Capability.xml");
                        capDes = textReader.ReadToEnd();
                        textReader.Close();
                        return capDes;
                    }
                    else
                    {
                        return "Sorry, the request method is not supported by this service.";
                    }                                  
                case "WFS":
                    if (httpRequest.QueryString["REQUEST"] == "GetFeature")
                    {
                        if ((httpRequest.QueryString["TypeName"] == "agent"))
                        {
                            switch (httpRequest.QueryString["FORMAT"])
                            { 
                                case "text":
                                    return m_agentLayer.GetFeaturesByText();
                                case "xml":
                                    return m_agentLayer.GetFeaturesByXML();
                            }
                        }
                        else
                            return "Query String is not supported";
                    }
                    break;
                default:
                    return "Unsupport Service";
            }
            return "Unsupport Service";
        }

        public void Initialise()
        {
            m_log.Info("[RESTPLUGIN]: " + Name + " cannot be default-initialized!");
            throw new PluginNotInitialisedException(Name);
        }
                
        public void Initialise(OpenSimBase openSim) {
            m_log.Info("[MapService]: initialized!");
            _app = openSim;
            _httpd = openSim.HttpServer;
            layerDict = new Dictionary<string, BaseLayer>();

            // read app config from OpenSim.MapService.ini
            LayerParams objLayerParams = new LayerParams();
            LayerParams terrainLayerParams = new LayerParams();
            LayerParams agentLayerParams = new LayerParams();
            try
            {
                m_config = new IniConfigSource("OpenSim.MapService.ini");
                getLayerParams(m_config.Configs["Object"], ref objLayerParams);
                getLayerParams(m_config.Configs["Terrain"], ref terrainLayerParams);
                getLayerParams(m_config.Configs["Agent"], ref agentLayerParams);                
            }
            catch (Exception e)
            {
                Console.WriteLine("Read Map Service config failure");
                Console.WriteLine(e.ToString());
            }
            
            // initailize new feature layers and register them
            m_objectLayer = new ObjectLayer(_app.SceneManager,  objLayerParams);
            m_terrainLayer = new TerrainLayer(_app.SceneManager, terrainLayerParams);
            m_agentLayer = new AgentLayer(_app.SceneManager, agentLayerParams);
            layerDict.Add(m_objectLayer.Name, m_objectLayer);
            layerDict.Add(m_terrainLayer.Name, m_terrainLayer);
            layerDict.Add(m_agentLayer.Name, m_agentLayer);
            
            // register ows handler
            string httpMethod = "GET";
            string path = "/ows";
            OWSStreamHandler h = new OWSStreamHandler(httpMethod, path, owsHandler);
            _httpd.AddStreamHandler(h);
        }

        public void PostInitialise() {
        }
    
        public void Dispose()
        {
        }

        private void getLayerParams(IConfig config, ref LayerParams layerParams)
        {
            layerParams.name = config.GetString("name");
            string[] sizes = config.GetString("size").Split(' ');
            layerParams.size_w = Int32.Parse(sizes[0]);
            layerParams.size_h = Int32.Parse(sizes[1]);
            layerParams.updateInterval = config.GetInt("update_interval");
        }
    }
}
