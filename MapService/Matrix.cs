using System;

namespace OpenSim.ApplicationPlugins.MapService
{
    /// <summary>
    /// A class representing a Matrix
    /// </summary>
    public class Matrix
    {
        double[,] matrix;
        int rows, columns;

        // This will not be called till before the application terminates
        ~Matrix()
        {            
        }

        public Matrix(int rows, int cols)
        {
            this.rows = rows;
            this.columns = cols;
            matrix = new double[rows, cols];
        }

        // Indexer for setting/getting internal array elements
        public double this[int i, int j]
        {
            set { matrix[i, j] = value; }
            get { return matrix[i, j]; }
        }

        // Return number of rows in the matrix
        public int Rows
        {
            get { return rows; }
        }

        // Return number of columns in the matrix
        public int Columns
        {
            get { return columns; }
        }

        public double Trace()
        {
            if (rows != columns)
                throw new Exception("Only square matrix has trace");

            double trace = 0;
            for (int i = 0; i < rows; i++)
                trace += this[i, i];
            return trace;
        }

        public static Matrix MatMul(Matrix A, Matrix B)
        {
            if (A.Columns != B.Rows)
                throw new Exception("First matrix col number must equal to second matrix row number");
            Matrix C = new Matrix(A.Rows, B.Columns);
            int i, j, k, size;
            double tmp;

            size = A.Columns;
            for (i = 0; i < A.Rows; i++)
            {
                for (j = 0; j < B.Columns; j++)
                {
                    tmp = C[i, j];
                    for (k = 0; k < size; k++)
                    {
                        tmp += A[i, k] * B[k, j];
                    }
                    C[i, j] = tmp;
                }
            }
            return C;
        }
        public static Matrix MatAdd(Matrix A, Matrix B)
        {
            if (A.Rows != B.Rows || A.Columns != B.Columns)
                throw new Exception("two matrces must have same rows and cols to add");

            Matrix C =  new Matrix(A.Rows, A.Columns);
            for (int i = 0; i < A.Rows; i++)
                for (int j = 0; j < A.Columns; j++)
                    C[i, j] = A[i, j] + B[i, j];
            return C;
        }
    }
}
