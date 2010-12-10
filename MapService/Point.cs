using System;
using System.Collections.Generic;
using System.Text;

namespace OpenSim.ApplicationPlugins.MapService
{
    public struct Point2D
    {
        public float x, y;
        public Point2D(float x, float y)
        {
            this.x = x; this.y = y;
        }
    }

    public struct Point3D
    {
        public float x, y, z;

        public Point3D(float x, float y, float z)
        {
            this.x = x; this.y = y; this.z = z;
        }

        public Point3D(Matrix mat)
        {
            if (mat.Columns != 1 || mat.Rows != 3)
                throw new Exception("mat rows must be 3 and cols must be 1");
            this.x = (float)mat[0, 0];
            this.y = (float)mat[1, 0];
            this.z = (float)mat[2, 0];
        }

        public Matrix ToMatrix()
        {
            Matrix A = new Matrix(3, 1);
            A[0, 0] = x;
            A[1, 0] = y;
            A[2, 0] = z;
            return A;
        }
    }
}
