# Quadtree Query In C

## Table of Contents

- [Introduction](#introduction)
- [Build](#build)
- [Search](#search)
- [Example](#example)
- [Tasks](#tasks)
- [Implementation](#implementation)

## Introduction

A quadtree is a data structure that stores *d*-dimensional points and enables efficient search for the stored points. We will only consider the case *d=2*. One particular quadtree which can be used to store 22-dimensional points is the point-region quadtree, simply referred as a *PR quadtree*. A binary tree can be defined as a finite set of nodes that are either empty or have a root and two binary trees *T(l)*​ and *T(r)* (the left and right subtree). A quadtree has a similar recursive definition: instead of two subtrees we have four subtrees, hence the name quad. This means that each node either has four children or is a leaf node. The four leaf nodes are often referred to as NW, NE, SW, SE (see the figure below).

![Quadtree Figure 1](./assets/quadtree_figure_1.png)

## Build

In a PR quadtree, each node represents a rectangle that covers part of the area that we wish to index. The root node covers the entire area. A PR quadtree is built recursively: we first start with an initial rectangle that should contain all the points we wish to store (how could you find a rectangle for *n* 2-dimensional points so that it contains all given datapoints?). To then construct a PR quadtree of a set of *n* points, we insert the points one by one and recursively subdivide a rectangle into four equally sized sub-rectangles whenever a rectangle would contain more than a single point. After the insertion process is complete, every leaf node is either coloured in *black* (contains a single datapoint) or white (indicating that the node is empty), and internal nodes are coloured in *grey* (see the figure above).

Alternatively, you could also use the following strategy: we insert all points into the rectangle and recursively partition each rectangle into 4 quadrants (sub-rectangles) until each rectangle contains at most one point.

The construction of a PR quadtree also leads to one important difference between them and binary trees: a binary tree depends on the order of value insertion whereas a PR quadtree is independent of the order of datapoint insertion. The reason is that the nodes are independent from the inserted datapoints (the rectangles are determined by the initial rectangle of the root node).

## Search

To enable efficient search, the rule is that *every leaf node in a PR quadtree either contains a single point or no point at all. Every node in a PR quadtree is either a leaf node or an internal node*, i.e., has four children representing a subdivision of the current rectangle into four equally sized quadrants. The region of an internal node always contains more than one point.

If we search for a single datapoint using its *2D* coordinates, we first check if the point lies in the quadtree. If it does, we recursively compute on each level the sub-rectangle that contains the coordinates of the datapoint to be searched until we reach a leaf node. The leaf node either contains the datapoint or it does not.

## Example

Here is a running example where we start with an empty PR quadtree, i.e., the we have a single white node, indicating we one leaf that is empty:

![Quadtree Figure 2](./assets/quadtree_figure_2.png)

We insert the first point *P*. We can simply colour the root node as black as we have only a single point.

![Quadtree Figure 3](./assets/quadtree_figure_3.png)

We insert a second point *Q*. Note that the colour changes from black to grey as the root node becomes an internal node (a leaf node can only hold a single datapoint but now holds two):

![Quadtree Figure 4](./assets/quadtree_figure_4.png)

Since the root is an internal node, we have to subdivide the rectangle into four sub-rectangles and have to add four children in the corresponding quadtree. However, since both points are still located in the same sub-rectangle NW, we colour this node as grey as well in the corresponding quadtree which has now a root and four leaves:

![Quadtree Figure 5](./assets/quadtree_figure_5.png)

Hence, we subdivide the NW rectangle further into four sub-rectangles. We add four more children nodes to the grey node that we coloured as grey in the previous step:

![Quadtree Figure 6](./assets/quadtree_figure_6.png)

Since this subdivision again does not lead to *P* and *Q* being in separate rectangles, we have to add another layer to the quadtree, i.e., subdivide the rectangle that includes both points once more into four rectangles. Since *P* and *Q* are now in separate rectangles, the corresponding nodes become leaves in the quadtree coloured in black and the two other children are simply coloured white. We have now completed the update of inserting *Q*:

![Quadtree Figure 7](./assets/quadtree_figure_7.png)

Finally, we insert a third point *R*. This is a simple case, because the leaf on the first level is empty, i.e., we simply colour the SE node on the first level of the quadtree as black:

![Quadtree Figure 8](./assets/quadtree_figure_8.png)

## Tasks

### Insertion

To insert a data point into a PR quadtree we start with the root node. If the PR quadtree is empty, i.e., the root node is coloured as white (which we represent as a pointer to NULL) and the point lies within the area that should be covered with the PR quadtree, the data point becomes the root. If the root is an internal node, we compute in which of the four children the data points lies (of course, if the data point lies outside of root rectangle, it is not inserted and an error message returned). Again, if the corresponding node is an internal node, we recursively repeat this process until we find a leaf node. If the leaf node is empty, we simply insert the point. If the leaf node is black, i.e., has already one node, we need to split the leaf node into four children, i.e., the existing black leaf node becomes a grey internal node. We repeat splitting the node until we find an internal node so that two of its children contain the existing and the newly inserted datapoint, i.e., are black leaf nodes. In other words: splitting a black leaf can also led to a recursive process.

### Find (Search)

To find a datapoint (and often its associated information), we simply traverse the PR quadtree by selecting the internal child node whose rectangle contains the location of the datapoint. The search stops once we have reached a leaf node. If the leaf node has stored the datapoint, we return its associated information; otherwise the search simply reports that the datapoint is not stored in the PR quadtree. 

### Range (Window) Query

An important query is to report all datapoints whose locations are contained in a given query rectangle. This query is typically called a range or window query. To run range query on a quadtree, we have the following recursive process: we start the root and check if the query rectangle overlaps with the root. If it does, we check which children nodes also overlap with query rectangle (this could be all of them for a centred query rectangle). Then we repeat this procedure for all internal children overlapping with the query rectangle. The recursion stop once we have reached the leaf level. We return the datapoints from all black leaf nodes whose datapoints are located within the query rectangle.

## Implementation

### Data Structures and Functions

To implement our own PR quadtree, we will need to set up some data structures and functions, we will likely need a number of structures and helper functions, these are not required and we do not need to follow the naming style or function as described, but we will likely find them useful in our implementation.

- **point2D** that stores the location of datapoint;
- **rectangle2D** that specifies a rectangle given an bottom-left 2D point and a upper-right 2D point;
- **dataPoint**, which is a structure that stores the location and associated information (i.e., the footpath information);
- **quadtreeNode**, which stores a 2D rectangle and 4 pointers referring to the four children SW, NW, NE and SE. 

We will also likely need the following functions:

- **inRectangle**: tests whether a given 2D point lies within the rectangle and returns 1 (TRUE) if it does. The function takes the point and the rectangle; note that the convention is that points are within a rectangle if they are on lower and right boundary but not on the top or left boundary. This enables us to construct a well-defined partition of the space if points lie on boundaries of the quadtree node rectangle.
- **rectangleOverlap**: tests whether two given rectangles overlap and returns 1 (TRUE) if they do.
- **determineQuadrant**: given a rectangle and point, returns the quadrant of the rectangle that the point lies in. The convention is *0=SW*, *1=NW*, *2=NE*, *3=SE* for the quadrant. You may like to implement helper functions which also select relevant pointers from the *quadtreeNode* given a quadrant. You may find writing an *enum quadrant* useful for implementing and using this function.
- **addPoint**: this function will add a datapoint given with its 2D coordinates to the quadtree. You will need to setup the logic discussed before.
- **searchPoint**: tests whether a datapoint given by its 2D coordinates lies within the rectangle and returns the datapoint (and its stored information) if it does and NULL otherwise.
- **rangeQuery**: takes a 2D rectangle as argument and returns all datapoints in the PR quadtree whose coordinates lie within the query rectangle.

### State 3 - Supporting Point Region Queries

In Stage 3, we will implement the basic functionality for a quadtree allowing the lookup of data by longitude (x) and latitude (y) pairs.

- Read data from the data file specified in the second command line argument. This may be stored unchanged from *dict1* or *dict2* in earlier implementations.
- Construct a quadtree from the stored data.
- Interpret and store the fourth, fifth, sixth and seventh arguments as *long double* values. The *strtold*  function should be used to achieve this.
- Accept co-ordinate pairs from stdin, search the constructed quadtree for the point region containing the co-ordinate pair and print all matching records to the output file. You may assume that all queries will be terminated by a new line. You should interpret these values as *double* values.
- In addition to outputting the record(s) to the output file, the list of quadrant directions followed from the root until the correct point region is found should be output to *stdout*.
- Our approach should insert each footpath's (*start_lon*, *start_lat*) and (*end_lon*, *end_lat*) pairs into the quadtree, allowing the footpath to be found from its start or end point.
- Where multiple footpaths are present in the found point region, footpaths should be printed in order of *footpath_id*.

The quadrants in relation to the dataset are:

![Stage 3 Figure 1](./assets/stage_3_figure_1.png)

For testing, it may be convenient to create a file of queries to be searched, one per line, and redirect the input from this file. Use the UNIX operator *<* to redirect input from a file.

#### Example Execution

```bash
make -B dict3
# ./dict3 stage datafile outputfile start_longitude start_latitude end_longitude end_latitude
./dict3 3 dataset_2.csv output.txt 144.969 -37.7975 144.971 -37.7955 < queryfile
```

#### Example Output

```c
144.97056424489568 -37.796155887263744
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
144.96941668057087 -37.79606116572821
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
144.95538810397605 -37.80355555400948
--> footpath_id: 19458 || address: Queensberry Street between Capel Street and Howard Street || clue_sa: North Melbourne || asset_type: Road Footway || deltaz: 1.44 || distance: 94.82 || grade1in: 65.8 || mcc_id: 1385878 || mccid_int: 20950 || rlmax: 35.75 || rlmin: 34.31 || segside: North || statusid: 2 || streetid: 1008 || street_group: 20939 || start_lat: -37.803461 || start_lon: 144.954243 || end_lat: -37.803556 || end_lon: 144.955388 || 
```

With the following output to *stdout*:

```c
144.97056424489568 -37.796155887263744 --> NE SW NE NE
144.96941668057087 -37.79606116572821 --> NE SW NE NW
144.95538810397605 -37.80355555400948 --> SW NW SE
```

### State 4 - Supporting Range Queries

In Stage 4, we will add the additional functionality to your quadtree to support range queries given by *(x, y)* co-ordinate pairs.

- Just as Stage 3, read in the dataset, store it in a dictionary and construct a quadtree on the stored data.
- For Stage 4, you should accept sets of pairs of co-ordinate *long double* type values from stdin, and efficiently use the quadtree to find all footpaths which are within the bounds of the query. You may assume that no blank lines will be present in the queries. 
- Output to *stdout* should include all directions searched in order, with each branch potentially containing points within the query bounds fully explored before proceeding to the next possible branch. Where multiple directions are possible, quadrants must be searched in the order *SW* , *NW* , *NE*, *SE* . Each direction must be separated by a space. The definitions of each quadrant are given in the table below.
- Similar to Stage 3, where multiple footpaths are returned by the range query, these should be sorted by *footpath_id*. Output each footpath only once for each query, even if both its "start" and "end" points both occur in the searched region.

#### Example Execution

```bash
make -B dict4
# ./dict4 stage datafile outputfile start_longitude start_latitude end_longitude end_latitude
./dict4 4 dataset_2.csv output.txt 144.968 -37.797 144.977 -37.79 < queryfile
```

#### Example Output

```c
144.968 -37.797 144.977 -37.79
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
--> footpath_id: 29996 || address:  || clue_sa: Carlton || asset_type: Road Footway || deltaz: 0.46 || distance: 54.51 || grade1in: 118.5 || mcc_id: 1388910 || mccid_int: 0 || rlmax: 24.91 || rlmin: 24.45 || segside:  || statusid: 0 || streetid: 0 || street_group: 29996 || start_lat: -37.793272 || start_lon: 144.975507 || end_lat: -37.794367 || end_lon: 144.975315 || 
144.9678 -37.79741 144.97202 -37.79382
--> footpath_id: 27665 || address: Palmerston Street between Rathdowne Street and Drummond Street || clue_sa: Carlton || asset_type: Road Footway || deltaz: 3.21 || distance: 94.55 || grade1in: 29.5 || mcc_id: 1384273 || mccid_int: 20684 || rlmax: 35.49 || rlmin: 32.28 || segside: North || statusid: 2 || streetid: 955 || street_group: 28597 || start_lat: -37.796156 || start_lon: 144.970564 || end_lat: -37.796061 || end_lon: 144.969417 || 
144.973 -37.795 144.976 -37.792
--> footpath_id: 29996 || address:  || clue_sa: Carlton || asset_type: Road Footway || deltaz: 0.46 || distance: 54.51 || grade1in: 118.5 || mcc_id: 1388910 || mccid_int: 0 || rlmax: 24.91 || rlmin: 24.45 || segside:  || statusid: 0 || streetid: 0 || street_group: 29996 || start_lat: -37.793272 || start_lon: 144.975507 || end_lat: -37.794367 || end_lon: 144.975315 || 
```

With the following output to *stdout*:

```c
144.968 -37.797 144.977 -37.79 --> SW SW SE NE SE
144.9678 -37.79741 144.97202 -37.79382 --> SW SW SE
144.973 -37.795 144.976 -37.792 --> NE SE
```