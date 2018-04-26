using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
[RequireComponent(typeof(MeshFilter))]
public class example : MonoBehaviour
{
    const int M = 6;
     List<int> feture_points = new List<int>{614,612,615,608,1091,1090,1085,1072,13,7,703,700,686,678,399,397,236,
         624,626,629,488,486,258,259,248,244,242,
         480,473,467,457,1396,1392,442,447,448,
         553,539,536,510,1493,560,
         312,320,1524,334,345,300,
         175,183,187,88,91,74,410,41,39,36,173,164,193,59,75,54,57,141};
    public double GetOutputX(double x)
    {

        double deltaX = 0.0;

        for (int i = 132; i < 132 + M*3; i=i+3)

            deltaX += Socket.face_fit.face_data_recv.face_fit_data[i] 
                * Mathf.Exp((float)(-1.0 * (x - Socket.face_fit.face_data_recv.face_fit_data[ i + 1]) 
                * (x - Socket.face_fit.face_data_recv.face_fit_data[i + 1]) 
                / (2 * Socket.face_fit.face_data_recv.face_fit_data[i + 2] 
                * Socket.face_fit.face_data_recv.face_fit_data[ i + 2])));

        return deltaX;

    }
    public double GetOutputY(double y)
    {

        double deltaY = 0.0;

        for (int i = 132 + M * 3; i < 132 + M * 6; i = i + 3)

            deltaY += Socket.face_fit.face_data_recv.face_fit_data[i]
                * Mathf.Exp((float)(-1.0 * (y - Socket.face_fit.face_data_recv.face_fit_data[i + 1])
                * (y - Socket.face_fit.face_data_recv.face_fit_data[i + 1])
                / (2 * Socket.face_fit.face_data_recv.face_fit_data[i + 2]
                * Socket.face_fit.face_data_recv.face_fit_data[i + 2])));

        return deltaY;

    }
    public int[] sort_vec = new int[66];
    public int[] sort_index = new int[66];
    private void Awake()
    {
        Dictionary<int, int> point2Vec = new Dictionary<int, int>();

        int n = 0;
        for (int i = 0; i <66; ++i)
        {
            point2Vec.Add(i, feture_points[i]);
        }
        var dicSort = from objDic in point2Vec orderby objDic.Value select objDic;
        foreach (KeyValuePair<int, int> item in dicSort)
        {
            sort_vec[n] = item.Value;
            sort_index[n] = item.Key;
            n++;
        }
        //for (int i = 0; i < 66; ++i)
        //{
        //    Debug.Log("index: " + sort_index[i] + " to Vec " + sort_vec[i]);
        //}

    }
    Vector3[] pre_vertices = new Vector3[1592];
    void Update()
    {
        Mesh mesh = this.transform.GetComponent<MeshFilter>().mesh;
        Vector3[] vertices = mesh.vertices;
        //int p = 0;
        //int flag = -1;
        //float maxheight = 0.0F;
        //while (p < vertices.Length)
        //{
        //    if (vertices[p].z > maxheight)
        //    {
        //        maxheight = vertices[p].z;
        //        flag = p;
        //    }
        //    p++;
        //}
        ////vertices[flag] += new Vector3(0, 0, maxheight);    
        //Debug.Log(vertices.Length);
        if (Socket.face_fit.face_data_recv.face_fit_data[170] == 66 && pre_vertices[0].x != 0)
        {
            double ScalesX = Socket.face_fit.face_data_recv.face_fit_data[168];
            double ScalesY = Socket.face_fit.face_data_recv.face_fit_data[169];
            //Debug.Log("32.2: "+GetOutputX(32.2) + " 53.2: " + GetOutputX(53.2));

            //Debug.Log("xscale : " + ScalesX + " yscale : " + ScalesY);
            for (int i = 0, k = 0; i < vertices.Length; i++)
            {
                if (i == sort_vec[k])
                {
                    vertices[i].x = pre_vertices[i].x - (float) (ScalesX * Socket.face_fit.face_data_recv.face_fit_data[sort_index[k] * 2]);
                    vertices[i].y = pre_vertices[i].y - (float) (ScalesY * Socket.face_fit.face_data_recv.face_fit_data[sort_index[k] * 2 + 1]);
                    k += (k < 65 ? 1 : 0);
                    vertices[i].z = vertices[i].z;
                }
                else
                {
                    //Debug.Log("rbf...");
                    float deltax = (float)(GetOutputX(pre_vertices[i].x));
                    float deltay = (float)(GetOutputY(pre_vertices[i].y));
                    vertices[i].x = pre_vertices[i].x - deltax;
                    vertices[i].y = pre_vertices[i].y - (float) (deltay);
                    //Debug.Log("rbf deltax:  " + deltax + "  rbf deltay: " + deltay);
                    vertices[i].z = vertices[i].z;
                }

            }
            
            Socket.face_fit.face_data_recv.face_fit_data[170] = 0;
        }
        pre_vertices = vertices;
        mesh.vertices = vertices;
        mesh.RecalculateNormals();
    }
    void OnGUI()
    {
        if (GUI.Button(new Rect(0, 0, 100, 30), "Mesh"))
        {
            Mesh mesh = this.transform.GetComponent<MeshFilter>().mesh;
            Vector3[] vertices = mesh.vertices;
            Debug.Log(vertices.Length);
   
            for (int i = 0,k = 0; i < vertices.Length; i++)
            {


                //Debug.Log("vertices[" + i + "].x  " + vertices[i].x + "   vertices[" + i + "].y  " + vertices[i].y + "   vertices[" + i + "].z  " + vertices[i].z);
                //if (i==175)
                //{
                //    vertices[i].x += 10;
                //    vertices[i].y += 10;
                //}
                //if (i == 410)
                //{
                //    vertices[i].x -= 10;
                //    vertices[i].y += 10;
                //}

                //if (i == sort_points[k])
                {
                    //k = k + (k < 65 ? 1 : 0);
                    GameObject sphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                    sphere.transform.position = vertices[i];
                    sphere.transform.name = "vec" + i.ToString();

                }
                    


            }
            mesh.vertices = vertices;


        }
    }
}