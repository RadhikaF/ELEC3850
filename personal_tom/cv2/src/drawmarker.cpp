#include <numeric>

#include "header.hpp"
#include "drawmarker.hpp"

using namespace std;
using namespace cv;


DrawMarker::DrawMarker(int marker_type, string marker_text, Scalar marker_colour, Point marker_p0, bool text_toggle){
    type = marker_type;
    show_text = text_toggle;
    p0 = marker_p0;
    text = marker_text;
    colour = marker_colour;
}

void DrawMarker::SetP1(Point marker_p1){
    p1 = marker_p1;
}