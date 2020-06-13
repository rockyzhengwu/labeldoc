#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>

namespace ocrmodel {
void tesseract_analysis(){
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    qDebug() << "tesseract" ;
    if (api->Init(nullptr, "shi_sim")) {
       fprintf(stderr, "Could not initialize tesseract.\n");
     }
 }
}
