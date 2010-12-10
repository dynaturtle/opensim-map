using System;
using System.Drawing;
using OpenSim.Region.Framework.Scenes;

namespace OpenSim.ApplicationPlugins.MapService
{
    public class TerrainLayer:BaseLayer
    {
        #region members
        private SceneManager _sceneManager;
        private int m_regionNumber;
        #endregion

        public TerrainLayer(SceneManager sceneManager, string name, Size cacheSize) : base(name, cacheSize)
        {
            _sceneManager = sceneManager;
            m_regionNumber = 0;
        }

        public TerrainLayer(SceneManager sceneManager, LayerParams layerParams)
            : base(layerParams)
        {
            _sceneManager = sceneManager;
            m_regionNumber = 0;
        }

        #region abstract method override
        protected override bool requireUpdate()
        {
            if (m_regionNumber != _sceneManager.Scenes.Count)
                return true;
            return false;
        }

        protected override void updateLayer()
        {
            return;
        }

        protected override Bitmap generateCacheBitmap(BBOXF spatialRegion, Size cacheSize)
        {           

            foreach (Scene scene in _sceneManager.Scenes)
            {
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
            }

            try
            {                
                //TODO add resolution 
                BBOX imgMapBBox = new BBOX(0, 0, (int)Math.Floor(m_layerRegion.Width - CRS.ROUNDERROR), (int)Math.Floor(m_layerRegion.Height - CRS.ROUNDERROR));
                Bitmap mapImg = new Bitmap(imgMapBBox.Width, imgMapBBox.Height);

                foreach (Scene scene in _sceneManager.Scenes)
                {
                    int x = (int)scene.RegionInfo.RegionLocX;
                    int y = (int)scene.RegionInfo.RegionLocY;

                    BBOX imgRegionBBox = CRS.Cartes2Image(new BBOXF(x * 256, y * 256, x * 256 + 256, y * 256 + 256),
                        mapImg.Size, m_layerRegion);                        
                    Bitmap regionImg = ImageToolBox.CreateBitmapFromMap(scene.Heightmap);
                    ImageToolBox.Replace(mapImg, regionImg, imgRegionBBox);
                }
                return mapImg;
            }
            catch (Exception e)
            {
                m_log.Info("Terrain generation failed!");
                m_log.Info("Exception is:" + e.ToString());
                return null;
            }
        }
        #endregion
    }
}
