# local-search-engine
This is a local search engine that uses Page Rank Algorithms, while modelling webpage and hyperlinks through directed graphs.

Webpages are modelled as vertices, and a hyperlink from webpage A to webpage B is represented as a directed edge from A to B.

The directory contains 3 CSV files that contain information about the webpage, one main cpp file, and one .h file that contains the class used. Class declaration and definition are in the same .h file.

### CSV Description
Keyword_File.csv: Keywords associated with each webpage
Numofimpressions_File.csv: For each webpage, there are two numbers associated with it. The first is the number of impressions (number of times the page appears on the search), and the second is the number of clicks (number of accesses to the page).
Webgraph_File.csv: Contains hyperlinks from each webpage to the next - listed in order as: source,destination
