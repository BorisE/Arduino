/*
 * New way of rendering web queries
 */
void handleSPIFSS(){
  digitalWrite(STATUS_LED, HIGH);

  if (!handleFileRead(server.uri()))                    // send it if it exists
      handleNotFound();                                 // or return 404

  printRequestData();
  digitalWrite(STATUS_LED, LOW);
}

// convert the file extension to the MIME type
String getContentType (String filename){
  if(filename.endsWith(".htm"))         return "text/html";
  else if(filename.endsWith(".html"))   return "text/html";
  else if(filename.endsWith(".css"))    return "text/css";
  else if(filename.endsWith(".js"))     return "application/javascript";
  else if(filename.endsWith(".png"))    return "image/png";
  else if(filename.endsWith(".gif"))    return "image/gif";
  else if(filename.endsWith(".jpg"))    return "image/jpeg";
  else if(filename.endsWith(".ico"))    return "image/x-icon";
  else if(filename.endsWith(".xml"))    return "text/xml";
  else if(filename.endsWith(".json"))   return "application/json";
  else if(filename.endsWith(".pdf"))    return "application/x-pdf";
  else if(filename.endsWith(".zip"))    return "application/x-zip";
  else if(filename.endsWith(".gz"))     return "application/x-gzip";
  return "text/plain";
}


bool handleFileRead (String path) { // send the right file to the client (if it exists)
  //Serial.println("[WEBSERVER] handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }else{
    Serial.print(F("[HTTP REQUEST] File Not Found: "));
    Serial.println(path);
    return false;                                         // If the file doesn't exist, return false
  }
}

/*
// not used
bool handleFileRead_compressed (String path){  // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){  // If the file exists, either as a compressed archive, or normal
    if(SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                         // Use the compressed version
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false;                                          // If the file doesn't exist, return false
}
*/
