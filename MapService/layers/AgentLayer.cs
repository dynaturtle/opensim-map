using System;
using System.Xml;
using System.IO;
using System.Drawing;
using System.Text;
using System.Collections.Generic;
using OpenSim.Region.Framework.Scenes;
using OpenMetaverse;

namespace OpenSim.ApplicationPlugins.MapService
{
    public class AgentLayer:BaseLayer
    {
        #region memebers
        private SceneManager _sceneManager;
        private Dictionary<UUID, AgentInfo> agentList;

        #endregion
        public AgentLayer(SceneManager sceneManager, string name, Size cacheSize)
            : base(name, cacheSize)
        {
            //update agent position each 1 sec
            this._sceneManager = sceneManager;
            this.m_pollingInterval = 1000;
            agentList = new Dictionary<UUID, AgentInfo>();
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="sceneManager"></param>
        /// <param name="layerParams"></param>
        public AgentLayer(SceneManager sceneManager, LayerParams layerParams): base(layerParams)
        {
                this._sceneManager = sceneManager;
                agentList = new Dictionary<UUID, AgentInfo>();
        }

        /// <summary>
        ///  return all agent position in GML format
        /// </summary>
        /// <returns></returns>
        public string GetFeaturesByXML()
        {
            Stream st = new MemoryStream();
            XmlTextWriter featureWriter = new XmlTextWriter(st, Encoding.UTF8);
            featureWriter.Formatting = Formatting.Indented;
            featureWriter.WriteStartDocument();
            // start write element FeatureCollection
            featureWriter.WriteStartElement("wfs", "FeatureCollection");
            featureWriter.WriteAttributeString("xmlns", "wfs", null, "http://www.opengis.net/wfs");
            featureWriter.WriteAttributeString("xmlns", "gml", null, "http://www.opengis.net/gml");
            featureWriter.WriteAttributeString("xmlns", "xsi", null, "http://www.w3.org/2001/XMLSchema-instance");
            featureWriter.WriteAttributeString("xsi", "schemaLocation", null, "http://www.opengis.net/wfs ../wfs/1.1.0/WFS.xsd");
            
            //start write element gml:boundedBy
            featureWriter.WriteStartElement("gml", "boundedBy");
            featureWriter.WriteStartElement("gml","Envelope");
            featureWriter.WriteAttributeString("srs", "http://www.opengis.net/gml/srs/epsg.xml#63266405");
            string lowerCorner = "";
            string upperCorner = "";
            featureWriter.WriteElementString("gml", "lowerCorner", lowerCorner);
            featureWriter.WriteElementString("gml", "upperCorner", upperCorner);
            featureWriter.WriteEndElement();//end of element gml:Envelope
            featureWriter.WriteEndElement(); //end of element gml:boundedBy

            foreach (var agent in agentList.Values)
            {
                featureWriter.WriteStartElement("gml", "featureMember");
                /*
                 * <gml:Point gml:id="p21" srsName="urn:ogc:def:crs:EPSG:6.6:4326">
                 * <gml:coordinates>45.67, 88.56</gml:coordinates>
                 * </gml:Point>         
                */
                featureWriter.WriteElementString("name", agent.name);
                featureWriter.WriteStartElement("gml", "Point");
                string posString = agent.position.X + "," + agent.position.Y + "," + agent.position.Z;
                featureWriter.WriteElementString("gml", "coordinates", null, posString);
                featureWriter.WriteEndElement();
                featureWriter.WriteEndElement();
            }
            featureWriter.WriteEndElement();// end write element FeatureCollection            
            featureWriter.WriteEndDocument();            
            featureWriter.Flush();
            
            byte[] buffer = new byte[st.Length];
            st.Seek(0, SeekOrigin.Begin);
            st.Read(buffer, 0, (int)st.Length);
            featureWriter.Close();
          
            return Encoding.UTF8.GetString(buffer);
        }

        public string GetFeaturesByText() 
        {
            string res = "";
            foreach (var agent in agentList.Values)
            {
                res += agent.name + "," + agent.position.X + "," + agent.position.Y + "," + agent.position.Z + "\n";
            }
            return res;
        }
        #region abstract method override
        protected override bool requireUpdate()
        {
            if (_sceneManager.Scenes == null || _sceneManager.Scenes.Count == 0)
                return false;
            return true;
        }

        protected override Bitmap generateCacheBitmap(BBOXF spatialRegion, Size cacheSize)
        {
            Bitmap mapImg = new Bitmap(cacheSize.Width, cacheSize.Height);
            Graphics gfx = Graphics.FromImage(mapImg);
            Pen pen = new Pen(Color.Red);
            Brush brush = Brushes.Red;

            // draw agent position on the map
            foreach (var agent in agentList.Values)
            {
                PointF[] cartesPoints = new PointF[1];
                PointF[] imgPoints = null;
                cartesPoints[0].X = agent.position.X; cartesPoints[0].Y = agent.position.Y;
                imgPoints = CRS.Cartes2Image(cartesPoints, cacheSize, spatialRegion);
                RectangleF rect = new RectangleF(imgPoints[0].X - 2, imgPoints[0].Y - 2, 20, 20);
                gfx.FillEllipse(brush, rect);                
            }            

            gfx.Dispose();
            pen.Dispose();
            return mapImg;
        }

        protected override void updateLayer()
        {
            agentList.Clear();
            foreach (Scene scene in _sceneManager.Scenes)
            {
                //update layer region
                int x = ((int)scene.RegionInfo.RegionLocX) * 256;
                int y = ((int)scene.RegionInfo.RegionLocY) * 256;
                if (m_layerRegion == null)
                {
                    m_layerRegion = new BBOXF(x, y, x + 256, y + 256);
                }
                else
                {
                    BBOXF newBBox = new BBOXF(x, y, x + 256, y + 256);
                    m_layerRegion.Extends(newBBox);
                }

                //update agent list
                List<ScenePresence> scenePresenceList = scene.GetAvatars();
                foreach (ScenePresence scenePresence in scenePresenceList)
                { 
                    Vector3 cartesPos = new Vector3();
                    cartesPos.X = scenePresence.AbsolutePosition.X + scene.RegionInfo.RegionLocX * 256;
                    cartesPos.Y = scenePresence.AbsolutePosition.Y + scene.RegionInfo.RegionLocY * 256;
                    cartesPos.Z = scenePresence.AbsolutePosition.Z;
                    AgentInfo tempAgent = new AgentInfo();
                    tempAgent.position = cartesPos;
                    tempAgent.name = scenePresence.Name;
                    agentList.Add(scenePresence.UUID, tempAgent);
                }
            }
        }
        #endregion
    }
}
