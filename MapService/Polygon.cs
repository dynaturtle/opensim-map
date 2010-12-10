using System;
using System.Collections.Generic;
using System.Drawing;


namespace OpenSim.ApplicationPlugins.MapService
{
    public class Polygon
    {
        private List<Point2D> pointList;

        public Polygon()
        { 
        }
        public Polygon(List<Point2D> pointList)
        {
            this.pointList = pointList;
        }

        public void AddPoint(Point2D pt, int index)
        {
            pointList.Insert(index, pt);
        }

        public override string ToString()
        {
            string res = "";
            foreach (var point in pointList)
            {
                res += "(" + point.x.ToString() + " " + point.y.ToString() + ")";
            }
            return res; 
        }

        public PointF[] ToPointFArray()
        {
            PointF[] ptFList = new PointF[pointList.Count];
            
            for (int i = 0; i < pointList.Count; i++)
            {
                ptFList[i] = new PointF(pointList[i].x, pointList[i].y);
            }

            return ptFList;
        }
    }
}
