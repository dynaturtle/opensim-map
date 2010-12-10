using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using OpenSim.Framework;
using OpenSim.Region.Framework.Scenes;
using OpenMetaverse;

namespace OpenSim.ApplicationPlugins.MapService
{
    public class ObjectLayer : BaseLayer
    {
        #region members        
        private SceneManager _sceneManager;              
        private List<Polygon> _polygonList;        
        private int regionNumber;
        #endregion

        private void GeneratePolygonList()
        {
            _polygonList.Clear();
            List<Scene> sceneList = _sceneManager.Scenes;
            lock (sceneList)
            {
                foreach (Scene scene in sceneList)
                {
                    List<EntityBase> objs = scene.GetEntities();
                    lock (objs)
                    {
                        foreach (EntityBase obj in objs)
                        {
                            if (obj is SceneObjectGroup)
                            {
                                SceneObjectGroup mapdot = (SceneObjectGroup)obj;
                                foreach (SceneObjectPart part in mapdot.Children.Values)
                                {
                                    if (part == null)
                                        continue;
                                    Vector3 pos = part.AbsolutePosition;
                                    Vector3 scale = part.Scale;
                                    Quaternion quaternion = part.RotationOffset;
                                    Polygon polygon = GeneratePolygon(scale, pos, quaternion);
                                    int gridX = (int)scene.RegionInfo.RegionLocX;
                                    int gridY = (int)scene.RegionInfo.RegionLocY;
                                    //CRS transformation
                                    polygon = CRS.Grid2Cartes(polygon, gridX, gridY);
                                    _polygonList.Add(polygon);
                                }
                            }
                        }
                    }
                }
            }            
        }

     
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="sceneManager"></param>
        public ObjectLayer(SceneManager sceneManager, string layerName, Size cacheSize) : base(layerName,cacheSize)
        {
            _sceneManager = sceneManager;
            _polygonList = new List<Polygon>();
            //TODO: layer region should be updated according to the real region change
            m_layerRegion = new BBOXF(256000, 256000, 256512, 256512);
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="sceneManager"></param>
        /// <param name="layerParams"></param>
        public ObjectLayer(SceneManager sceneManager, LayerParams layerParams) : base(layerParams)
        {
            _sceneManager = sceneManager;
            _polygonList = new List<Polygon>();
            m_layerRegion = new BBOXF(256000, 256000, 256512, 256512);
        }

        #region static members and methods
        private static Point2D tempOrigin;
        static public Matrix QuaternionToMatrix(Quaternion quat)
        { 
            Matrix A = new Matrix(3, 3);
            A[0, 0] = -1 + 2 * quat.W * quat.W + 2 * quat.Z * quat.Z;
            A[0, 1] = 2 * (quat.W * quat.X - quat.Y * quat.Z);
            A[0, 2] = 2 * (quat.W * quat.Y + quat.X * quat.Z);
            A[1, 0] = 2 * (quat.W * quat.X + quat.Y * quat.Z);
            A[1, 1] = -1 + 2 * quat.X * quat.X + 2 * quat.Z * quat.Z;
            A[1, 2] = 2 * (quat.X * quat.Y - quat.W * quat.Z);
            A[2, 0] = 2 * (quat.W * quat.Y - quat.X * quat.Z);
            A[2, 1] = 2 * (quat.W * quat.Z + quat.X * quat.Y);
            A[2, 2] = -1 + 2 * quat.Y * quat.Y + 2 * quat.Z * quat.Z;
            return A;
        }

        private static int ComparePointByY(Point2D pt1, Point2D pt2)
        {
            if (pt1.y != pt2.y)
                return pt1.y.CompareTo(pt2.y);
            else
                return pt1.x.CompareTo(pt2.x);
        }

        private static double Radius(double x, double y)
        {
            if (x == 0)
            {
                if (y > 0)
                    return Math.PI * 0.5;
                else return Math.PI * 1.5;
            }
            double rad = Math.Atan(y / x);
            if (x < 0)
                rad = rad + Math.PI;
            if (x > 0 && y < 0)
                rad = rad + Math.PI * 2;
            return rad;
        }

        private static double Distance(double x, double y)
        {
            return Math.Sqrt(x * x + y * y);
        }
        private static int ComparePointByPolarAngle(Point2D pt1, Point2D pt2)
        {
            double rad1 = Radius(pt1.x - tempOrigin.x, pt1.y - tempOrigin.y);
            double rad2 = Radius(pt2.x - tempOrigin.x, pt2.y - tempOrigin.y);
            return rad1.CompareTo(rad2);             
        }

        private static bool TurnLeft(Point2D pt1, Point2D pt2, Point2D pt3)
        {
            Vector2 vt1 = new Vector2(pt2.x - pt1.x, pt2.y - pt1.y);
            Vector2 vt2 = new Vector2(pt3.x - pt2.x, pt3.y - pt2.y);
            double det = vt1.X * vt2.Y - vt1.Y * vt2.X;
            if (det > 0)
                return true;
            else
                return false;
        }

        // Calculte the 2Dconvcex hull of the point list on XY plane
        private static Polygon ConvenxHull(List<Point2D> pointList)
        {
            List<Point2D> polyPointList = new List<Point2D>();
            pointList.Sort(ComparePointByY);

            Point2D p0 = new Point2D(pointList[0].x, pointList[0].y);
            polyPointList.Add(p0);
            pointList.RemoveAt(0);

            tempOrigin = p0;            
            pointList.Sort(ComparePointByPolarAngle);
            
            //reogranize the pointlist remove duplicate points which have same polar angles
            int index = 0;
            while (index + 1 < pointList.Count)
            {
                double rad1 = Radius(pointList[index].x - tempOrigin.x, pointList[index].y - tempOrigin.y);
                double rad2 = Radius(pointList[index + 1].x - tempOrigin.x, pointList[index + 1].y - tempOrigin.y);
                if (rad1 == rad2)
                {
                    double dist1 = Distance(pointList[index].x - tempOrigin.x, pointList[index].y - tempOrigin.y);
                    double dist2 = Distance(pointList[index + 1].x - tempOrigin.x, pointList[index + 1].y - tempOrigin.y);

                    if (dist1 < dist2)
                        pointList.RemoveAt(index);
                    else
                        pointList.RemoveAt(index + 1);
                }
                else
                    index++;
            }
            Point2D p1 = pointList[0]; pointList.RemoveAt(0);
            Point2D p2 = pointList[0]; pointList.RemoveAt(0);

            polyPointList.Add(p1);
            polyPointList.Add(p2);

            for (int i = 0; i < pointList.Count; i++)
            {
                while (TurnLeft(polyPointList[polyPointList.Count - 2], polyPointList[polyPointList.Count - 1], pointList[i]) == false)
                    polyPointList.RemoveAt(polyPointList.Count - 1);
                polyPointList.Add(pointList[i]);
            }

            return new Polygon(polyPointList);
        }

        public static Polygon GeneratePolygon(Vector3 scale, Vector3 pos, Quaternion quat)
        {
            Matrix rotationMat = QuaternionToMatrix(quat);
            Matrix transMat = new Matrix(3,1);
            transMat[0,0] = pos.X;
            transMat[1,0] = pos.Y;
            transMat[2,0] = pos.Z;


            // generate original Point list
            List<Point3D> oriPointList = new List<Point3D>(8);
            oriPointList.Add(new Point3D(scale.X / 2, scale.Y / 2, scale.Z / 2));
            oriPointList.Add(new Point3D(scale.X / 2, scale.Y / 2, -scale.Z / 2));
            oriPointList.Add(new Point3D(scale.X / 2, -scale.Y / 2, scale.Z / 2));
            oriPointList.Add(new Point3D(scale.X / 2, -scale.Y / 2, -scale.Z / 2));
            oriPointList.Add(new Point3D(-scale.X / 2, scale.Y / 2, scale.Z / 2));
            oriPointList.Add(new Point3D(-scale.X / 2, scale.Y / 2, -scale.Z / 2));
            oriPointList.Add(new Point3D(-scale.X / 2, -scale.Y / 2, scale.Z / 2));
            oriPointList.Add(new Point3D(-scale.X / 2, -scale.Y / 2, -scale.Z / 2));

            // rotate the point by the rotationMat
            List<Point3D> rotPointList = new List<Point3D>(8);
            foreach (var point in oriPointList)
            {
                Point3D rPoint = new Point3D(Matrix.MatMul(rotationMat, point.ToMatrix()));
                rotPointList.Add(rPoint);
            }
            
            // translate the point by translatMat
            List<Point3D> transPointList = new List<Point3D>(8);
            foreach (var point in rotPointList)
            { 
                Point3D tPoint = new Point3D(Matrix.MatAdd(transMat, point.ToMatrix()));
                transPointList.Add(tPoint);
            }

            // project points to xy plane
            List<Point2D> point2dList = new List<Point2D>();
            foreach (var point3d in transPointList)
            {
                Point2D point2d = new Point2D(point3d.x, point3d.y);
                point2dList.Add(point2d);
            }

            Polygon polygon = ConvenxHull(point2dList);
            return polygon;
        }

        #endregion


        #region abstract method override
        protected override bool requireUpdate()
        {
            if (m_layerRegion!=null &&  regionNumber != _sceneManager.Scenes.Count)
                return true;
            return false;
        }

        protected override void updateLayer()
        {
            regionNumber = _sceneManager.Scenes.Count;
            GeneratePolygonList();            
        }

        protected override Bitmap generateCacheBitmap(BBOXF spatialRegion, Size cacheSize)
        {
            Bitmap mapImg = new Bitmap(cacheSize.Width, cacheSize.Height);
            Graphics gs = Graphics.FromImage(mapImg);
            Pen pen = new Pen(Color.Blue, 3);

            foreach (var polygon in _polygonList)
            {
                PointF[] cartesPoints = polygon.ToPointFArray();
                PointF[] imgPoints = CRS.Cartes2Image(cartesPoints, cacheSize, spatialRegion);
                gs.DrawPolygon(pen, imgPoints);
            }

            gs.Dispose();
            pen.Dispose();
            return mapImg;
        }

        #endregion

    }
}
