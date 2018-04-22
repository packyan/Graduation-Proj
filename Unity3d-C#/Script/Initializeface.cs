using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using System.Runtime.InteropServices;

public class Initializeface : MonoBehaviour {
    // Use this for initialization
    const int date_size = 111 * 2;
    const int current_node_num = 111;
    const int k = 5;
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
    public struct facevec
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = date_size)]
        //test points
        public Vector3[] face_vector;
    }

    public static class face_vec
    {
        public static facevec fv;
    }

    
    public Vector3[] allFaceNode = new Vector3[current_node_num];
    public static bool run_onec = true;
    public static int[] knn = new int[current_node_num * k];
    public GameObject planeDraw;//这里是Mesh Filter组件所在的物体，要拖进去~
    public Mesh planeDraw_mesh;
    private LineRenderer lineRenderer;
    private int LengthOfLineRenderer = 0;
    private void Awake()
    {
        
        face_vec.fv.face_vector = new Vector3[current_node_num];
        //planeDraw_mesh = planeDraw.GetComponent<Mesh>();
        for (int x = 0; x < current_node_num; x++)
            {
                GameObject sphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                //sphere.AddComponent<Rigidbody>(); //no gravity
                sphere.tag = "faceNode";
                sphere.transform.position = new Vector3(0, 0, 0);
            }
    }
    void Start () {
        ////添加LineRenderer组件  
        //lineRenderer = gameObject.AddComponent<LineRenderer>();
        ////设置材质  
        //lineRenderer.material = new Material(Shader.Find("Materials/Texture0"));
        ////设置颜色  
        //lineRenderer.SetColors(Color.black, Color.black);
        //lineRenderer.SetWidth(0.02f, 0.02f);
        //lineRenderer.SetVertexCount(66 + 1);

    }
    protected void addnode()
    {

    }
    public void drawPlane(Vector3[] vertice, int[] knn)//入口参数：存放顶点信息的vector3数组
    {
        int numberOfTriangles = vertice.Length - 2;//三角形的数量等于顶点数减2
        int[] triangles = new int[numberOfTriangles * 3];//triangles数组大小等于三角形数量乘3
        int f = 0, b = vertice.Length - 1;//f记录前半部分遍历位置，b记录后半部分遍历位置
        for (int i = 0; i < numberOfTriangles; i++)//每次给 triangles数组中的三个元素赋值，共赋值
        { //numberOfTriangles次
            triangles[3 * i] = i;
            triangles[3 * i+1] = knn[i];
            triangles[3 * i + 2] = knn[i+1] ;//正向赋值，对于i=1赋值为：0,1,2
            //if (i % 2 == 1)
            //{
            //    triangles[3 * i - 3] = f++;
            //    triangles[3 * i - 2] = f;
            //    triangles[3 * i - 1] = b;//正向赋值，对于i=1赋值为：0,1,2
            //}
            //else
            //{
            //    triangles[3 * i - 1] = b--;
            //    triangles[3 * i - 2] = b;
            //    triangles[3 * i - 3] = f;//逆向赋值，对于i=2赋值为：1,5,6
            //}
        }
        planeDraw_mesh.vertices = vertice;
        planeDraw_mesh.triangles = triangles;//最后给mesh这个属性的vertices和triangles数组赋 值就ok~
    }

    // Update is called once per frame
    void Update () {
        GameObject[] facenodes;
        int i = 0;
        facenodes = GameObject.FindGameObjectsWithTag("faceNode"); // find all face nodes
        Debug.Log("drawing face...");
        //restore all face nodes to allFaceNode vector3
        for (int j = 0; j < current_node_num; ++j)
        {

                double x = Socket.face_fit.face_data_recv.face_fit_data[2 * j];
                double y = Socket.face_fit.face_data_recv.face_fit_data[2 * j + 1];
                allFaceNode[j] = new Vector3((float)x, (float)y);
        }
        // knn find k nearest neighbors. only need run onec.
        
       
        if (run_onec && allFaceNode[0].x != 0.0)
        {
            for (int m = 0; m < current_node_num; ++m)
            {
                Dictionary<int, double> dist = new Dictionary<int, double>();
                Debug.Log("processing: " + m);
                for (int n = 0; n < current_node_num; ++n)
                {
                    //double[] dist = new double[feture_point - 1];             
                    if (m != n) dist.Add(n, Mathf.Sqrt(Mathf.Pow(allFaceNode[m].x - allFaceNode[n].x, 2) + Mathf.Pow(allFaceNode[m].y - allFaceNode[n].y, 2)));
                }
                var dicSort = from objDic in dist orderby objDic.Value  select objDic;
                //    Dictionary<int, double> dists = (from temp in dist orderby temp.Key ascending select temp).
                //ToDictionary(pair => pair.Key, pair => pair.Value); //descending

                //foreach (KeyValuePair<int, double> item in dicSort)

                //{

                //    Debug.Log(item.Key + "   " + item.Value);

                //}
                //Debug.Log(m + " has been processed");
                int kk = 0;
                foreach (KeyValuePair<int, double> item in dicSort)
                {
                    ++kk;
                    knn[k*m + kk - 1] = item.Key;
                    if (kk == k) break;
                }

            }
                
            run_onec = false;
            //for(int knnnum = 0; knnnum < feture_point*k ; knnnum = knnnum + k)
            //{
            //    Debug.Log(knn[knnnum] + "  and  " + knn[knnnum + 1]);
            //}
            Debug.Log("KNN completed");

        }
        foreach (GameObject node in facenodes)
        {
            node.transform.name = "node" + i.ToString();
            //Debug.Log("package head: " + BitConverter.ToString(Socket.face_fit.face_data_recv.packages_head));
            //double x = Socket.face_fit.face_data_recv.face_fit_data[2 * i];
           // double y = Socket.face_fit.face_data_recv.face_fit_data[2 * i + 1];
           // face_vec.fv.face_vector[i] = new Vector3((float)x, (float)y);
            //Debug.Log("face point: ( " + x + ',' + y + " )");
            //node.transform.localPosition = Vector3.MoveTowards(gameObject.transform.localPosition, new Vector3((float) x, (float) y), step);
            node.transform.position = allFaceNode[i];
            
            //drawing lines
            //from facenode[i] = k nearest neighbors.



            //allFaceNode[i] = new Vector3((float)x, (float)y);
            ++i;
            //if (i < 66)
            //{
            //    node.transform.name = "node" + i.ToString();
            //    //Debug.Log("package head: " + BitConverter.ToString(Socket.face_fit.face_data_recv.packages_head));
            //    double x = Socket.face_fit.face_data_recv.face_fit_data[2 * i];
            //    double y = Socket.face_fit.face_data_recv.face_fit_data[2 * i + 1]; 
            //    face_vec.fv.face_vector[i] = new Vector3((float)x, (float)y);
            //    //Debug.Log("face point: ( " + x + ',' + y + " )");
            //    //node.transform.localPosition = Vector3.MoveTowards(gameObject.transform.localPosition, new Vector3((float) x, (float) y), step);
            //    node.transform.position = new Vector3((float)x, (float)y);
            //    allFaceNode[i] = new Vector3((float)x, (float)y);
            //    ++i;
            //}
        }
        if (run_onec == false && allFaceNode[0].x != 0.0)
        {
            for (int ii = 0; ii < current_node_num; ++ii)
            {
                for(int j = 0; j < k; ++j)
                Debug.DrawLine(allFaceNode[ii], allFaceNode[knn[k * ii +j]], Color.black);
            }
            Debug.Log("drawing lines");

        }
           
        //Debug.Log("ready to drawing plane ");

        //if (allFaceNode[0].x != 0.0 && run_onec == false)
        //{
        //    Debug.Log("drawing Plane...");
        //    drawPlane(allFaceNode,knn);
        //}
    }


}
