# Unity3D C\#
## Socket
### Usage:
1. `socket.cs` is used to communicata with the client, and receive Face-fit data form a linux client.
2. `Initializeface` is used to initialize face nodes in Unity3D and drive these nodes to make face animation.

## Logs:
1. 2018.04.09 First test finished. Just simpely receive data from client, and drive some spheres.
2. 2018.04.21 `face-fit` use linear interp to add additional face point, it's purely to increase the number of points. So the facial animation looks very hard. Optimize the server to receive separated data packets.