using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;

namespace OpenSim.ApplicationPlugins.MapService
{
    public class BBOX
    {
        public int MinX, MinY, MaxX, MaxY;
        public BBOX()
        {
            MinX = 0;
            MinY = 0;
            MaxX = 0;
            MaxY = 0;
        }

        public BBOX(int minX, int minY, int maxX, int maxY)
        {
            MinX = minX;
            MinY = minY;
            MaxX = maxX;
            MaxY = maxY;
        }

        public BBOX(string boxStr)
        {
            string[] boxParams = boxStr.Split(',');
            MinX = (int)float.Parse(boxParams[0]);
            MinY = (int)float.Parse(boxParams[1]);
            MaxX = (int)float.Parse(boxParams[2]);
            MaxY = (int)float.Parse(boxParams[3]);
        }

        public void Extends(int x, int y)
        {
            if (x < MinX) MinX = x;
            if (x > MaxX) MaxX = x;
            if (y < MinY) MinY = y;
            if (y > MaxY) MaxY = y;
        }

        public void Extends(BBOX bbox)
        {
            MinX = Math.Min(MinX, bbox.MinX);
            MinY = Math.Min(MinY, bbox.MinY);
            MaxX = Math.Max(MaxX, bbox.MaxX);
            MaxY = Math.Max(MaxY, bbox.MaxY);
        }

        public int Width
        { 
            get{ return MaxX - MinX + 1;}
        }

        public int Height
        {
            get { return MaxY - MinY + 1; }
        }

        public Rectangle ToRectangle()
        {
            Rectangle rect = new Rectangle(MinX,MinY, Width - 1,Height - 1);
            return rect;
        }
    }
}
