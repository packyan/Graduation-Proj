using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class other_models : MonoBehaviour {
    Mesh mesh;
    Vector3[] pre_vertices  = new Vector3[1];
    private void Awake()
    {
        mesh = this.transform.GetComponent<MeshFilter>().mesh;
        Vector3[] pre_vertices = new Vector3[mesh.vertices.Length];
    }
    // Use this for initialization
    void Start () {
		
	}
    const int M = 6;
    public double GetOutputX(double x)
    {

        double deltaX = 0.0;

        for (int i = 132; i < 132 + M * 3; i = i + 3)

            deltaX += Socket.face_fit.face_data_recv.face_fit_data[i]
                * Mathf.Exp((float)(-1.0 * (x - Socket.face_fit.face_data_recv.face_fit_data[i + 1])
                * (x - Socket.face_fit.face_data_recv.face_fit_data[i + 1])
                / (2 * Socket.face_fit.face_data_recv.face_fit_data[i + 2]
                * Socket.face_fit.face_data_recv.face_fit_data[i + 2])));

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
        // Update is called once per frame
        
    void Update () {

       
        Vector3[] vertices = mesh.vertices;
        if (pre_vertices[0].x != 0 && Socket.face_fit.face_data_recv.face_fit_data[170] == 66)
        {
            double ScalesX = Socket.face_fit.face_data_recv.face_fit_data[168];
            double ScalesY = Socket.face_fit.face_data_recv.face_fit_data[169];
            //Debug.Log("32.2: "+GetOutputX(32.2) + " 53.2: " + GetOutputX(53.2));

            //Debug.Log("xscale : " + ScalesX + " yscale : " + ScalesY);
            for (int i = 0; i < vertices.Length; i++)
            {
               
                {
                    //Debug.Log("rbf...");
                    float deltax = (float)(GetOutputX(pre_vertices[i].x));
                    float deltay = (float)(GetOutputY(pre_vertices[i].y));
                    vertices[i].x = pre_vertices[i].x - deltax;
                    vertices[i].y = pre_vertices[i].y - (float)(deltay);
                    //Debug.Log("rbf deltax:  " + deltax + "  rbf deltay: " + deltay);
                    vertices[i].z = vertices[i].z;
                }

            }
        }
        pre_vertices = vertices;
        mesh.vertices = vertices;
        mesh.RecalculateNormals();
    }
}
