//======================================================
//Assignment 3
//TITLE: A* Algorithm
//AUTHOR: MARZIA ABDUL RAHIM
//ID: MAR976-6287645
//* This assignment is written by me from my understanding of A* Algorithm
//======================================================
#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
using namespace std;

//STRUCT
struct Vertice{
    int label=-1;
    double xCo=-1, yCo=-1;
};
struct Node {
    int label=0;                    //name of the node-> will be used to compare if we reach the goal 
    int indexV=0;                   //the actual index of this node ->will be used to track the parent using index
    double totalWeight=0;           //parent weight plus the current weight ->used to calculate the huristic and total weight 
    double huristic=0;              //distance plus the total weight -> will be used to sort the heap base on it
    int parent;                     //parent node of this nodes-> will be used to back track to print the path 
};

//GLOBAL VARIABLES
int nVertices=0, nEdges=0, start=0, goal=0;                              
int indexH=0, indexP=0;

//FUCNTIONS
double secondShortestPath(Vertice *vList,double** graph,Node *Heap,Node *path); //the reason that it is double because I want to calculate the total once 
double aStarSearch(Vertice *vList,double** graph,Node *Heap,Node *path);            // and I don't to want run a sperate loop to get the total weight.
void siftDown(int i,Node *Heap);
void siftUp(int,Node *Heap);
double calculatehuristic(int indexIn,int,Vertice *vList,Node *Heap);
void makeGraph();

int main() {
    Vertice *vList;
    double** graph;
    Node *Heap;
    Node *path;
    //FILE NAME
    string fileName="";
    cout<<"FILE NAME: ";
    cin>>fileName;

    //READ FILE
    ifstream inData;
    inData.open(fileName);
    if(!inData.good()) {
        cerr<<"Can not open the file!"<<endl;
        return 1;
    }
    else {
        //GETTING THE SIZE (FIRST LINE OF THE FILE)
        inData>>nVertices>>nEdges;
        vList=new Vertice[nVertices];
        graph=new double* [nVertices];
        for(int i=0;i<nVertices;i++) {
            graph[i]=new double[nVertices];
            for(int j=0;j<nVertices;j++) {
                graph[i][j]=-1;
            }
        }
        Heap=new Node[nVertices];
        path=new Node[nVertices];
        //VERTICES 
        for(int i=0;i<nVertices;i++) {
            inData>>vList[i].label>>vList[i].xCo>>vList[i].yCo;
        }
        //EDGE
        int label1;
        int label2;
        double weight;
        for(int i=0;i<nEdges;i++) {
            inData>>label1>>label2>>weight;
            if(graph[label1-1][label2-1]==-1 && graph[label2-1][label1-1]==-1) {
                graph[label1-1][label2-1]=graph[label2-1][label1-1]=weight;
            }
            else {
                graph[label1-1][label2-1]=min(graph[label1-1][label2-1],weight);
                graph[label2-1][label1-1]=min(graph[label2-1][label1-1],weight);
            }
        }
        //LAST LINE
        inData>>start>>goal;
    }
    inData.close();

    //FIRST PATH
    double total=aStarSearch(vList,graph,Heap,path);
    cout<<"1.SHORTEST PATH: ";
    for(int i=0;i<indexP;i++) {
        cout<<path[i].label<<"  ";
    }
    cout<<"\nLENGTH OF PATH: "<<total<<endl;

    //SECOND PATH
    total=secondShortestPath(vList,graph,Heap,path);
    cout<<"2.SHORTEST PATH: ";
    for(int i=0;i<indexP;i++) {
        cout<<path[i].label<<"  ";
    }
    cout<<"\nLENGTH OF PATH: "<<total<<endl;

    //DELETE THE DYNAMIC MEMORY 
    for(int j=0;j<nVertices;j++)
    {
        delete [] graph[j];
    }
    delete[] vList;
    delete[] graph;
    delete[] Heap;
    delete[] path;

    return 0;
}
//==================================================================================
//===============================SECOND SHORTEST PATH===============================
double secondShortestPath(Vertice *vList,double** graph,Node *Heap,Node *path) {
    //copy the first shortest path and the index
    Node* firstPath=path;
    int indexfirstPath=indexP;      
    //set minimum
    int minTotal=123456789;  
    //create a temp path to save the minimum path 
    Node finalPath[100];   
    int finalPathIndex=0;    

    //Empty the array of path and save it into array of indexes
    for(int i=1;i<=indexfirstPath-1;i++) {          //from one because we skip start //indexP-1 because we will skip the goal also
        double** graphTemp=graph;
        //UPDATE THE GRAPH
        graphTemp[firstPath[i].parent][firstPath[i].indexV]=graphTemp[firstPath[i].indexV][firstPath[i].parent]=-1;
        //A STAR SEARCH
        double total=aStarSearch(vList,graphTemp,Heap,path);
        //check for the minimum
        if(minTotal>total) {
            minTotal=total;
            for(int j=0;j<indexP;j++) {
                finalPath[j]=path[j];
            }
            finalPathIndex=indexP;
        }
    }

    //copy the shortest path to the Path and update the indexP
    for(int j=0;j<finalPathIndex;j++) {
        path[j]=finalPath[j];
    }
    indexP=finalPathIndex;

    return minTotal;
}
//=================================A STAR ALGORITHM=================================
double aStarSearch(Vertice *vList,double** graph,Node *Heap,Node *path) {
    //reset the nodes for the second shortest path
    indexP=0;
    indexH=0;
    //IF START AND GOAL IS SAME 
    if(start==goal) {
        cout<<"We are already at destination!"<<endl;
        return 0;
    }
	bool reachedGoal=false;
    //INITIALISE THE PARAMETER OF THE FIRST NODE
    Node node;
    node.label=start;
    node.huristic=calculatehuristic(start-1,goal-1,vList,Heap);
    node.totalWeight=0;
    node.indexV=start-1;
	node.parent=-1;
    Heap[indexH++]=node;
    //ADD OTHER NODES CONNECTED TO START IN HEAP
    Node child;
    for(int i=0;i<nVertices;i++) {
        if(graph[node.indexV][i]>0 && i!=start-1) {
            child.label=vList[i].label;
            child.indexV=i;
            child.parent=node.indexV;
            child.totalWeight=graph[node.indexV][i];                                    //TotalWeight=Weight+Parent.Weight
            child.huristic=calculatehuristic(i,goal-1,vList,Heap)+child.totalWeight;
			if(child.label==vList[goal-1].label) {
                reachedGoal=true;
                break;
            }
            Heap[indexH]=child;
            siftUp(indexH,Heap);                                                                     //shift up base on Heuristic
            indexH++;
        }
    }
	//ADD THE START NODE TO THE PATH
    path[indexP++]=Heap[0];
    Heap[0]=Heap[indexH-1];
    indexH--;
    siftDown(0,Heap);
    if(reachedGoal) {
        path[indexP++]=child;
        return child.totalWeight;
    }

    //RUN TILL WE REACH THE DESTINATION
    while(indexH!=0 && !reachedGoal) {
        Node CurrenNode=Heap[0];
        //ADD THE REACHABLE VERTICES TO HEAP
        for(int i=0;i<nVertices;i++) {
            if(graph[CurrenNode.indexV][i]>0 && i!=start-1) {
                //NEW NODE
                node.label=vList[i].label;
                node.indexV=i;
                node.parent=CurrenNode.indexV;
                node.totalWeight=graph[CurrenNode.indexV][i]+CurrenNode.totalWeight;             //TotalWeight=Weight+Parent.Weight
                node.huristic=calculatehuristic(i,goal-1,vList,Heap)+node.totalWeight;
                bool flag=false;
                //CHECK IF IT IS THE GOAL 
                if(node.label==vList[goal-1].label) {
                    reachedGoal=true;
                    break;
                }
                //CHECK IF IS ALREADY IN THE HEAP
                for(int j=0;j<indexH;j++) {
                    if(Heap[j].label==node.label) {
                        if(node.totalWeight<Heap[j].totalWeight) {
                            Heap[j]=node;
                            siftUp(j,Heap);                                                      //shift up base on Heuristic
                        }
                        flag=true;
                    }
                }
                //CHECK IF IT ALREADY IN THE PATH 
                for(int i=0;i<indexP;i++) {
                    if (path[i].label==node.label) {
                        flag=true;
                    }
                }
                //OTHERWISE JUST ADD IT TO THE HEAP
                if(flag==false) {
                    Heap[indexH]=node;
                    siftUp(indexH,Heap);                                                          //shift up base on Heuristic
                    indexH++;
                }   
            }
        }
        //ADD IT TO THE PATH 
        path[indexP++]=Heap[0];              
        Heap[0]=Heap[indexH-1];
        indexH--;
        siftDown(0,Heap);

        if(reachedGoal) {
            //Add the goal to path 
            path[indexP++]=node;
            //Remove the paths which does not lead to goal
            Node* temp=new Node[indexP];
            int j=0;
            temp[j]=path[indexP-1];

            for(int i=indexP-2;i>=0;i--) {
                if(temp[j].parent==path[i].indexV) {
                    temp[++j]=path[i];
                }
            }
            //Copy back the path and calculate the total weight 
            indexP=0;
            for(int i=0;i<=j;i++) {
                path[indexP++]=temp[j-i];
            }
            return node.totalWeight;                        //I already calculate this for the huristic so I can just return the total weight of the goal node.
        }
    }
    return 0;
}
//=========================================HEAP(Base on Huristic which is distance+totalWeight)=========================================
void siftUp(int i,Node *Heap) {
    if(i==1) {
        return;
    }
    int parent=i/2;
    if(Heap[parent].huristic<Heap[i].huristic) {
        return;
    }
    else {
        swap(Heap[i],Heap[parent]);
        siftUp(parent,Heap);
    }
}
void siftDown(int currentIndex,Node *Heap)
{
	// move element i down to its correct postion
	int child=(currentIndex*2)+1;		//left Child
	if(child+1<=indexH && Heap[child].huristic>Heap[child+1].huristic)
	{
		child++;
	}
	if(child<=indexH && Heap[currentIndex].huristic>Heap[child].huristic)
	{
		swap(Heap[currentIndex],Heap[child]);
		siftDown(child,Heap);
	}
}
//=========================================huristic=========================================
double calculatehuristic(int indexIn,int otherindex,Vertice *vList,Node *Heap) {
    for(int i=0;i<indexH;i++) {
        if(indexIn==Heap[i].indexV) {
            return Heap[i].huristic;
        }
    }
    double point1= (vList[indexIn].xCo-vList[otherindex].xCo)*(vList[indexIn].xCo-vList[otherindex].xCo);
    double point2= (vList[indexIn].yCo-vList[otherindex].yCo)*(vList[indexIn].yCo-vList[otherindex].yCo);
    return (double)sqrt(point1+point2);
}