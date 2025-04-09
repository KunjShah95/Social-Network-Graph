# Social Network Graph

A C++ implementation of a social network graph with various graph-based algorithms for relationship analysis.

## Features

- **User Management**: Add users to the network
- **Relationship Management**: Create bi-directional friendships between users
- **Network Analysis**:
  - Find mutual friends between two users
  - Suggest potential friends based on mutual connections
  - Find shortest path between users using BFS
  - Find shortest path between users using Dijkstra's algorithm

## Implementation Details

The social network is implemented as an adjacency list using a map of sets. Each user in the network can have multiple friends, and the relationship is bi-directional.

The project showcases several important graph algorithms:
- Set operations for finding mutual friends
- Friend-of-friend algorithm for suggesting new connections
- Breadth-First Search (BFS) for finding shortest paths
- Dijkstra's algorithm for finding shortest paths in weighted graphs

## How to Use

Compile the Main.cpp file with a C++ compiler:

```
g++ -o social_network Main.cpp
```

Run the executable:

```
./social_network
```

## Sample Output

The program creates a sample network with users (Alice, Bob, Charlie, etc.) and demonstrates various operations like finding mutual friends, suggesting potential connections, and finding shortest paths between users.