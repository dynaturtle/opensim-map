using System;
using System.Collections.Generic;
using System.Text;

namespace OpenSim.ApplicationPlugins.MapService
{
    public class LayerParams
    {
        public int updateInterval;
        public int size_w;
        public int size_h;
        public string name;
        public LayerParams()
        {
            updateInterval = 1000;
            size_w = 1000;
            size_h = 1000;
            name = "default";
        }
    }
}
