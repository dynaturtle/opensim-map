using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using OpenSim.Region.Framework.Interfaces;
using OpenSim.Region.Framework.Scenes;

namespace OpenSim.ApplicationPlugins.MapService
{
    class SizeNotMatchException : ApplicationException
    {
        public SizeNotMatchException() :
            base("the replace region size must be same as the part bitmap")
        { 
        }
    }
    class ImageToolBox
    {
        /// <summary>
        ///  replace the specified region of source bitmap with part bitmap
        /// </summary>
        public static void Replace(Bitmap source, Bitmap part, BBOX region)
        {
            if (part.Height != region.Height || part.Width != region.Width)
                throw new SizeNotMatchException();

            for (int i = 0; i < region.Height; i++)
                for (int j = 0; j < region.Width; j++)
                { 
                    Color color = part.GetPixel(j,i);
                    source.SetPixel(region.MinX + j, region.MinY + i, color);
                }
        }

        public static Bitmap CreateBitmapFromMap(ITerrainChannel map)
        {
            Bitmap gradientmapLd = new Bitmap("defaultstripe.png");

            int pallete = gradientmapLd.Height;

            Bitmap bmp = new Bitmap(map.Width, map.Height);
            Color[] colours = new Color[pallete];

            for (int i = 0; i < pallete; i++)
            {
                colours[i] = gradientmapLd.GetPixel(0, i);
            }

            for (int y = 0; y < map.Height; y++)
            {
                for (int x = 0; x < map.Width; x++)
                {
                    // 512 is the largest possible height before colours clamp
                    int colorindex = (int)(Math.Max(Math.Min(1.0, map[x, y] / 512.0), 0.0) * (pallete - 1));

                    // Handle error conditions
                    if (colorindex > pallete - 1 || colorindex < 0)
                        bmp.SetPixel(x, map.Height - y - 1, Color.Red);
                    else
                        bmp.SetPixel(x, map.Height - y - 1, colours[colorindex]);
                }
            }
            return bmp;
        }
    }
}
