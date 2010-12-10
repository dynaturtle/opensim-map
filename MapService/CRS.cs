using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace OpenSim.ApplicationPlugins.MapService
{
    /// <summary>
    ///  provide convertor for transformations among different coordinate reference system
    /// </summary>
    class CRS
    {
        public static float ROUNDERROR = 1e-5f;
        /// <summary>
        /// 
        /// </summary>
        /// <param name="cartesBbox"></param>
        /// <param name="imgOrigin">the cartes coordinate of img origin</param>
        /// <param name="xRes">resolution of x-axis, defined as 1 pixel/spatial_width</param>
        /// <param name="yRes">resolution of y-axis, defined as 1 pixel/spatial_height</param>
        /// <returns></returns>
        public static BBOX Cartes2Image(BBOXF cartesBbox, Size imgSize, BBOXF  spatialRegion)
        {
           
            PointF[] points = new PointF[2];
            points[0] = new PointF(cartesBbox.MinX, cartesBbox.MinY);            
            points[1] = new PointF(cartesBbox.MaxX, cartesBbox.MaxY);
            PointF[] imgPoints = Cartes2Image(points, imgSize, spatialRegion);

            BBOX imgBbox = new BBOX();
            imgBbox.MinX = (int)Math.Floor(imgPoints[0].X);
            imgBbox.MinY = (int)Math.Floor(imgPoints[1].Y);
            imgBbox.MaxX = (int)Math.Floor(imgPoints[1].X - ROUNDERROR);
            imgBbox.MaxY = (int)Math.Floor(imgPoints[0].Y - ROUNDERROR);

            return imgBbox;
        }

        public static PointF[] Cartes2Image(PointF[] cartesPoints, Size imgSize, BBOXF spatialRegion)
        {
            PointF[] imgPoints = new PointF[cartesPoints.Length];
            Matrix mat = new Matrix(3, 3);
            mat[0, 0] = 1.0 * imgSize.Width / (spatialRegion.Width); mat[0, 1] = 0; mat[0, 2] = -1.0 * imgSize.Width * spatialRegion.MinX / (spatialRegion.Width);
            mat[1, 0] = 0; mat[1, 1] = -1.0 * imgSize.Height / spatialRegion.Height; mat[1, 2] = 1.0 * spatialRegion.MaxY * imgSize.Height / spatialRegion.Height;
            mat[2, 0] = 0; mat[2, 1] = 0; mat[2, 2] = 1;

            for (int i = 0; i < cartesPoints.Length; i++)
            {
                Matrix cPt = new Matrix(3, 1);
                cPt[0, 0] = cartesPoints[i].X;
                cPt[1, 0] = cartesPoints[i].Y;
                cPt[2, 0] = 1;
                Matrix iPt = Matrix.MatMul(mat, cPt);
                imgPoints[i] = new PointF((float)iPt[0, 0], (float)iPt[1, 0]);
            }

            return imgPoints;
        }

        public static Polygon Grid2Cartes(Polygon gPolygon, int gridX, int gridY)
        {
            PointF[] pointList = gPolygon.ToPointFArray();
            List<Point2D> cPointList = new List<Point2D>();
            
            for (int i = 0; i < pointList.Length; i++)
            {
                Point2D pt = new Point2D(gridX * 256 + pointList[i].X, gridY * 256 + pointList[i].Y);
                cPointList.Add(pt);
            }
            Polygon cPolygon = new Polygon(cPointList);
            return cPolygon;
        }
    }
}
