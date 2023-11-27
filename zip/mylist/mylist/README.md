# Customisation
## JSON data
The URL for getting a wodge of data (in JSON format) is
https://api.tfl.gov.uk/StopPoint/910GACTONML/Arrivals  
where *910GACTONML* is the Station ID for Acton Town Main Line.  
To find the station id for your station do a search https://api.tfl.gov.uk/StopPoint/Search/????? where *?????* is the name (or partial name) of what you are looking for. Note the search is case insensitive.  
Scan the returned JSON file for the ID you need.
### Memory Usage
The raw data returned can be quite large e.g. for Earls Court with 2 lines and 6 platforms the returned JSON file is around 26K bytes. This must be deserialised to a JSON object. By using a JSON filter we can drastically reduce the size of the final JSON object. For example raw Earls Court data, unfiltered, results in a JSON object 12K in size. Filtering by 3 fields, platform name,
destination name and arrival time yiels a final JSON object 2.1K in size.  
Use https://arduinojson.org/v6/assistant/#/step1 to estimate memory needed.
## JSON filter
Edit the *filterKeys* array in <b>jsonHandler.cpp</b> to define which fields in JSON you wish to retain.
## Refresh Rate
Assuming that arrival time will be expressed in minutes it is not worth getting new data from TFL any faster than that. Taking data at faster rates e.g. every 15 seconds shows that arrival times do not necessarily change between samples. Also one can see that arrival times can increase, indicating delays.  
JSON data is not always in chronological order. This project sorts by the *timeToStation* field to place trains in chronological order.
