#include "Arduino.h"
/**
html page that is sent from the server to the client. The rest of the page is rendered on the client side 
using javascript that is hosted in AWS S3. 
**/
String pageString = String("<!DOCTYPE html><title>HotDog</title><body><script src=https://s3-us-west-2.amazonaws.com/yourbucket/yourscript3.js></script><script src=https://s3-us-west-2.amazonaws.com/yourbucket/yourscript.js></script><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.2/css/bootstrap.min.css\"><div id=inputbox onclick=myPicker.show() style=margin:auto;padding:0;height:1080px><div><input id=mypicker onchange=update(this.jscolor) type=hidden value=ffcc00></div></div><script src=https://s3-us-west-2.amazonaws.com/yourbucket/yourscript2.js></script>");
