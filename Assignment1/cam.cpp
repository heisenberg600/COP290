#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class mouseSelection{
    public:
    Mat image;
    vector<Point2f> inPoints;

    // function to be given to set mouse callback (PROBLEM HERE)
    static void selectingPoints(int event, int x, int y, int flag, void* param) {

        mouseSelection *mouse = reinterpret_cast<mouseSelection*>(param);
        if (event == EVENT_LBUTTONDOWN) {

            circle(mouse->image,Point2f(x,y),5,Scalar(0,0,0),FILLED);        
            imshow("Original Frame",mouse->image);

            if (mouse->inPoints.size() < 4) {
                mouse->inPoints.push_back(Point2f(x,y));
            }
        }
        else if (event == EVENT_RBUTTONDOWN){
            if (mouse->inPoints.size()>0){
                mouse->inPoints.pop_back();
            }
        }  

    }
    //function to sort the points in our prefrence always gives a  quad enumeration
    void pointsSort(){
        Point2f mid= Point2f(0,0);
        for (auto it = inPoints.begin(); it != inPoints.end(); it++){ 
            mid.x+=(*it).x/4;
            mid.y+=(*it).y/4;
        }
        int i=0;
        for (int j=2;j<4; j++){ 
            if (inPoints[j].y<mid.y){
                if (inPoints[i].y<mid.y){
                    i++;
                }
                swap(inPoints[j],inPoints[i]);
            }        
        }
        if (inPoints[0].x>mid.x){
            swap(inPoints[0],inPoints[1]);          
        }
        if (inPoints[2].x<mid.x){
            swap(inPoints[2],inPoints[3]);          
        }           
    }
    //function to draw the quad formed by the points to better realise the selection
    void drawQuad(){
        for(int i=0;i<3;i++){
        line(image,inPoints[i],inPoints[i+1],Scalar(0,0,0),4,LINE_8);
    }
    line(image,inPoints[3],inPoints[0],Scalar(0,0,0),4,LINE_8);  
    imshow("Original Frame",image);
    }
};


int main(int argc, char* argv[]) {

    // reading the image (grayscale) 
    Mat sourcImg = imread("empty.jpg",0);
    Mat wrapImg = Mat::zeros(sourcImg.size(),CV_8UC3);
    if (sourcImg.empty()) {
        cout << "Unable to open the source image file - Check if empty.jpg present in the folder or not." << endl;
        cin.get(); //wait for any key press
        return -1;
    }
    Mat sourcImgcopy=sourcImg.clone();
    mouseSelection mouse;
    mouse.image = sourcImgcopy;

    // display grayscaled source image
    imshow("Original Frame", sourcImgcopy);


    // maping points given on course website
    vector<Point2f> mapPoints;
    mapPoints.push_back(Point2f(472,52));
    mapPoints.push_back(Point2f(800,52));
    mapPoints.push_back(Point2f(800,830));
    mapPoints.push_back(Point2f(472,830));

          
    
    setMouseCallback("Original Frame", mouse.selectingPoints, &mouse);
    waitKey(0); // ---> to be changed.
    mouse.pointsSort();
    mouse.drawQuad();

    waitKey(0);


    // do homography
    Mat wrapedMatrix = findHomography(mouse.inPoints, mapPoints);
    warpPerspective(sourcImg, wrapImg, wrapedMatrix, sourcImg.size());

    destroyWindow("Original Frame");
    imshow("Wraped Image", wrapImg);


    int xdimension,ydimension;
    int c=waitKey() & 0xFF;
    if (c==109){
        cout<<"Please Enter the x dimension of the croped image to be formed:\n";
        cin>>xdimension;
        cout<<"Please Enter the y dimension of the croped image to be formed:\n";
        cin>>ydimension;
    }
    else{
        xdimension=329;
        ydimension=779;
    }

    Mat cropImage(ydimension,xdimension,CV_8UC3,Scalar(0,0,0));

    vector<Point2f> cropPoints;
    cropPoints.push_back(Point2f(0,0));  
    cropPoints.push_back(Point2f(xdimension-1,0));
    cropPoints.push_back(Point2f(xdimension-1,ydimension-1));
    cropPoints.push_back(Point2f(0,ydimension-1));

    wrapedMatrix = findHomography(mapPoints,cropPoints);
    warpPerspective(wrapImg, cropImage, wrapedMatrix, cropImage.size());
    destroyWindow("Wraped Image");
    imshow("Croped Image",cropImage);
    waitKey(0);
    
}
