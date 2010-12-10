using System;
using System.Threading;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Reflection;
using log4net;

namespace OpenSim.ApplicationPlugins.MapService
{
    public abstract class BaseLayer : IDisposable
    {
        private string m_name;
        protected BBOXF m_layerRegion;
        protected Bitmap m_imgCache;
        protected Size m_imgCacheSize;        
        protected DateTime m_lastUpdated;
        protected int m_pollingInterval = 20000;
        protected static readonly ILog m_log =
           LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
        public string Name
        {
            get { return m_name;}
        }

        public BaseLayer(string name, Size cacheSize)
        {
            this.m_name = name;
            this.m_imgCacheSize = cacheSize;
            this.m_imgCache = null;
            ThreadStart layerMonitorStart = new ThreadStart(LayerMonitor);
            Thread layerMonitorThread = new Thread(layerMonitorStart);
            layerMonitorThread.Name = string.Format("{0} monitor thread",name);
            layerMonitorThread.Start();
        }

        public BaseLayer(LayerParams layerParams)
        {
            this.m_name = layerParams.name;
            this.m_imgCacheSize = new Size(layerParams.size_w, layerParams.size_h);
            this.m_pollingInterval = layerParams.updateInterval;
            this.m_imgCache = null;
            ThreadStart layerMonitorStart = new ThreadStart(LayerMonitor);
            Thread layerMonitorThread = new Thread(layerMonitorStart);
            layerMonitorThread.Name = string.Format("{0} monitor thread", this.m_name);
            layerMonitorThread.Start();
        }

        public void LayerMonitor()
        {            
            Thread.Sleep(m_pollingInterval);
            while (true)
            {
                if (requireUpdate())
                {
                    updateLayer();
                                   
                    if (m_imgCache != null)
                        m_imgCache.Dispose();
                    m_imgCache = generateCacheBitmap(m_layerRegion, m_imgCacheSize);
                }
                Thread.Sleep(m_pollingInterval);
            }
        }

        abstract protected bool requireUpdate();
        abstract protected void updateLayer();
        abstract protected Bitmap generateCacheBitmap(BBOXF spatialRegion, Size cacheSize);

        public Bitmap GetRegionByBitmap(BBOXF spatialRegion, Size size)
        {            
            m_log.Info(string.Format("[Map Service]: Request region {0}", spatialRegion.ToString()));
            Bitmap regionBMP = new Bitmap(size.Width, size.Height);            
            
            lock (m_imgCache)
            {
                // Get the area from image cache
                BBOX imgRegion = CRS.Cartes2Image(spatialRegion, m_imgCache.Size, m_layerRegion);
                Rectangle srcRect = imgRegion.ToRectangle();
                Rectangle destRect = new Rectangle(0,0,size.Width - 1, size.Height - 1);
                Graphics gfx = Graphics.FromImage((Image)regionBMP);
                gfx.InterpolationMode = InterpolationMode.HighQualityBicubic;
                gfx.DrawImage(m_imgCache, destRect, srcRect,  GraphicsUnit.Pixel);
                gfx.Dispose();
            }

            return regionBMP;
        }

        public void Dispose()
        {
            m_imgCache.Dispose();
        }
    }
}
