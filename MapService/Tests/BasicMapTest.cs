using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using OpenMetaverse;
using OpenSim.ApplicationPlugins.MapService;

namespace OpenSim.ApplicationPlugins.MapService.Tests
{
    public class BasicMapTest
    {
        [Test]
        public void TestPrimToPolygon()
        {
            Vector3 scale = new Vector3(10,10,10);
            Vector3 pos = new Vector3(0, 0, 0);
            Quaternion quat = new Quaternion(1,1,1,0);
            Polygon polygon = ObjectLayer.GeneratePolygon(scale, pos,quat);
            Console.WriteLine(polygon.ToString());
            Assert.AreEqual(polygon.ToString(),"(-5 -35)(15 -15)(25 5)(5 35)(-15 15)(-25 -5)");

            scale = new Vector3(2.818983f, 3.444397f, 3.444397f);
            pos = new Vector3(181.7685f, 242.7976f, 41.09935f);
            quat = new Quaternion(0f, 0f, 0.9415441f, 0.3368899f);
            polygon = ObjectLayer.GeneratePolygon(scale, pos, quat);
            Console.WriteLine(polygon.ToString());
        }
    }
}
