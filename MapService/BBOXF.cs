using System;
using System.Collections.Generic;
using System.Text;

namespace OpenSim.ApplicationPlugins.MapService
{
    public class BBOXF
    {
        public float MinX, MinY, MaxX, MaxY;
        public BBOXF()
        {
            MinX = 0;
            MinY = 0;
            MaxX = 0;
            MaxY = 0;
        }

        public BBOXF(float minX, float minY, float maxX, float maxY)
        {
            MinX = minX;
            MinY = minY;
            MaxX = maxX;
            MaxY = maxY;
        }

        public BBOXF(string boxStr)
        {
            string[] boxParams = boxStr.Split(',');
            MinX = float.Parse(boxParams[0]);
            MinY = float.Parse(boxParams[1]);
            MaxX = float.Parse(boxParams[2]);
            MaxY = float.Parse(boxParams[3]);
        }
        public void Extends(int x, int y)
        {
            if (x < MinX) MinX = x;
            if (x > MaxX) MaxX = x;
            if (y < MinY) MinY = y;
            if (y > MaxY) MaxY = y;
        }

        public void Extends(BBOXF bbox)
        {
            MinX = Math.Min(MinX, bbox.MinX);
            MinY = Math.Min(MinY, bbox.MinY);
            MaxX = Math.Max(MaxX, bbox.MaxX);
            MaxY = Math.Max(MaxY, bbox.MaxY);
        }

        public float Width
        {
            get { return MaxX - MinX; }
        }

        public float Height
        {
            get { return MaxY - MinY; }
        }

        public override string ToString()
        {
            string[] boxParams = new string[4];
            boxParams[0] = MinX.ToString();
            boxParams[1] = MinY.ToString();
            boxParams[2] = MaxX.ToString();
            boxParams[3] = MaxY.ToString();
            return string.Format("{0}, {1},{2},{3}", boxParams);
        }
    }
}
