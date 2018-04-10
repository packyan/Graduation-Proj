using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Initializeface : MonoBehaviour {
    // Use this for initialization
    private void Awake()
    {
        for (int y = 0; y < 11; y++)
        {
            for (int x = 0; x < 6; x++)
            {
                GameObject sphere = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                //sphere.AddComponent<Rigidbody>(); //no gravity
                sphere.tag = "faceNode";
                sphere.transform.position = new Vector3(x, y, 0);
            }
        }


    }
    void Start () {
     
         

    }
	// Update is called once per frame
	void Update () {
        GameObject[] facenodes;
        float speed = 100; // need to test
        float step = speed * Time.deltaTime;
        int i = 0;
        facenodes = GameObject.FindGameObjectsWithTag("faceNode"); // find all face nodes
        Debug.Log("drawing face...");
        foreach (GameObject node in facenodes)
        {
            Debug.Log("package head: " + BitConverter.ToString(Socket.face_fit.face_data_recv.packages_head));
            double x = Socket.face_fit.face_data_recv.face_fit_data[2*i];
            double y = Socket.face_fit.face_data_recv.face_fit_data[2*i+1];
            //Debug.Log("face point: ( " + x + ',' + y + " )");
            //node.transform.localPosition = Vector3.MoveTowards(gameObject.transform.localPosition, new Vector3((float) x, (float) y), step);
            node.transform.position =new Vector3((float)x, (float)y);
            ++i;
        }

    }
}
