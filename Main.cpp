#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <queue>
#include <list>
#include <limits>
#include <utility>

using namespace std;

// Class representing a social network as an adjacency list graph
class SocialNetwork {
private:
    map<string, set<string>> adj; // Adjacency list representation of the graph using sets for friends

public:
    void addUser(const string &userName);
    void addFriendship(const string &user1, const string &user2);
    set<string> getFriends(const string &userName) const;
    void printGraph() const;

    // Advanced graph operations
    set<string> getMutualFriends(const string &user1, const string &user2);
    vector<pair<string, int>> suggestFriends(const string &userName);
    pair<int, list<string>> shortestPathBFS(const string &startUser, const string &endUser);
    pair<int, list<string>> shortestPathDijkstra(const string &startUser, const string &endUser);
};

// Adds a new user to the social network
void SocialNetwork::addUser(const string &userName) {
    if (adj.find(userName) == adj.end()) {
        adj[userName] = set<string>(); // Create an empty set for the new user's friends
        cout << "User '" << userName << "' added." << endl;
    }
}

// Creates a bidirectional friendship between two users
void SocialNetwork::addFriendship(const string &user1, const string &user2) {
    if (adj.find(user1) != adj.end() && adj.find(user2) != adj.end()) {
        adj[user1].insert(user2);
        adj[user2].insert(user1);
        cout << "Friendship added between '" << user1 << "' and '" << user2 << "'." << endl;
    } else {
        cout << "One or both users do not exist." << endl;
    }
}

// Returns all friends of a specific user
set<string> SocialNetwork::getFriends(const string &userName) const {
    auto it = adj.find(userName);
    if (it != adj.end()) {
        return it->second;
    }
    cout << "User '" << userName << "' not found." << endl;
    return set<string>();
}

// Displays the entire social network structure
void SocialNetwork::printGraph() const {
    cout << "\n--- Social Network Graph ---" << endl;
    if (adj.empty()) {
        cout << "The network is empty." << endl;
        return;
    }
    for (const auto &pair : adj) {
        cout << "'" << pair.first << "' is friends with: {";
        string separator = "";
        for (const string &friendName : pair.second) {
            cout << separator << "'" << friendName << "'";
            separator = ", ";
        }
        cout << "}" << endl;
    }
    cout << "----------------------------\n" << endl;
}

// Finds common friends between two users using set intersection
set<string> SocialNetwork::getMutualFriends(const string &user1, const string &user2) {
    set<string> mutualFriends;
    auto it1 = adj.find(user1);
    auto it2 = adj.find(user2);

    if (it1 == adj.end() || it2 == adj.end()) {
        cout << "Error: One or both users ('" << user1 << "', '" << user2 << "') not found for mutual friends calculation." << endl;
        return mutualFriends;
    }

    const set<string> &friends1 = it1->second;
    const set<string> &friends2 = it2->second;

    // Find intersection of two friend sets
    std::set_intersection(friends1.begin(), friends1.end(), friends2.begin(), friends2.end(), 
                          std::inserter(mutualFriends, mutualFriends.begin()));
    return mutualFriends;
}

// Suggests potential friends based on mutual connections (friend-of-friend algorithm)
vector<pair<string, int>> SocialNetwork::suggestFriends(const string &userName) {
    map<string, int> suggestionCounts;
    vector<pair<string, int>> sortedSuggestions;

    auto it = adj.find(userName);
    if (it == adj.end()) {
        cout << "Error: User '" << userName << "' not found for friend suggestions." << endl;
        return sortedSuggestions;
    }

    const set<string> &directFriends = it->second;

    // Iterate through each direct friend
    for (const string &friendName : directFriends) {
        auto friendIt = adj.find(friendName);
        if (friendIt == adj.end()) continue;

        // Look at friends-of-friends
        for (const string &potentialFriend : friendIt->second) {
            if (potentialFriend != userName && directFriends.find(potentialFriend) == directFriends.end()) {
                if (adj.count(potentialFriend)) {
                    suggestionCounts[potentialFriend]++;
                }
            }
        }
    }

    // Convert to vector for sorting
    for (const auto &pair : suggestionCounts) {
        sortedSuggestions.push_back(pair);
    }

    // Sort by number of mutual connections (descending) and name (ascending)
    std::sort(sortedSuggestions.begin(), sortedSuggestions.end(),
              [](const pair<string, int> &a, const pair<string, int> &b) {
                  if (a.second != b.second) {
                      return a.second > b.second;
                  }
                  return a.first < b.first;
              });

    return sortedSuggestions;
}

// Finds shortest path between users using Breadth-First Search
pair<int, list<string>> SocialNetwork::shortestPathBFS(const string &startUser, const string &endUser) {
    list<string> path;
    int distance = -1; // -1 indicates no path found

    // Validate input users exist
    if (adj.find(startUser) == adj.end()) {
        cout << "Error: Start user '" << startUser << "' not found for BFS." << endl;
        return {distance, path};
    }
    if (adj.find(endUser) == adj.end()) {
        cout << "Error: End user '" << endUser << "' not found for BFS." << endl;
        return {distance, path};
    }

    // Special case: path to self
    if (startUser == endUser) {
        path.push_back(startUser);
        return {0, path};
    }

    // BFS algorithm implementation
    queue<string> q;
    map<string, string> parent; // For path reconstruction
    map<string, int> dist;      // Track distances

    // Initialize distances
    for (const auto &pair : adj) {
        dist[pair.first] = -1; // -1 means unvisited
        parent[pair.first] = "";
    }

    // Start BFS from startUser
    q.push(startUser);
    dist[startUser] = 0;

    bool found = false;
    while (!q.empty() && !found) {
        string currentUser = q.front();
        q.pop();

        // Explore all neighbors
        for (const string &neighbor : adj.at(currentUser)) {
            if (dist.count(neighbor) && dist[neighbor] == -1) {
                dist[neighbor] = dist[currentUser] + 1;
                parent[neighbor] = currentUser;
                q.push(neighbor);

                if (neighbor == endUser) {
                    found = true;
                    distance = dist[neighbor];
                    break;
                }
            }
        }
    }

    // Reconstruct path if one was found
    if (found) {
        string current = endUser;
        while (!current.empty()) {
            path.push_front(current);
            current = parent[current];
        }
    } else {
        cout << "BFS: No path found between '" << startUser << "' and '" << endUser << "'." << endl;
    }

    return {distance, path};
}

// Finds shortest path using Dijkstra's algorithm (optimal for weighted graphs)
pair<int, list<string>> SocialNetwork::shortestPathDijkstra(const string &startUser, const string &endUser) {
    list<string> path;
    int finalDistance = -1; // Default: no path found

    // Validate input users exist
    if (adj.find(startUser) == adj.end()) {
        cout << "Error: Start user '" << startUser << "' not found for Dijkstra." << endl;
        return {finalDistance, path};
    }
    if (adj.find(endUser) == adj.end()) {
        cout << "Error: End user '" << endUser << "' not found for Dijkstra." << endl;
        return {finalDistance, path};
    }

    // Special case: path to self
    if (startUser == endUser) {
        path.push_back(startUser);
        return {0, path};
    }

    // Dijkstra algorithm implementation
    map<string, int> dist;          // Track shortest distance to each node
    map<string, string> parent;     // For path reconstruction
    priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq; // Min-priority queue

    // Define INF constant for "infinity" distance
    const int INF = numeric_limits<int>::max();

    // Initialize distances
    for (const auto &pair : adj) {
        dist[pair.first] = INF; // INF means infinity (unreachable initially)
        parent[pair.first] = "";
    }

    // Start with startUser
    dist[startUser] = 0;
    pq.push({0, startUser});

    bool found = false;
    while (!pq.empty()) {
        int d = pq.top().first;
        string u = pq.top().second;
        pq.pop();

        // Skip outdated entries in priority queue
        if (d > dist[u]) {
            continue;
        }

        // Check if we've reached the destination
        if (u == endUser) {
            found = true;
            finalDistance = dist[u];
            break;
        }

        // Explore all neighbors
        for (const string &v : adj.at(u)) {
            int weight = 1; // Assuming unweighted graph (each edge has weight 1)

            if (dist.count(v)) {
                // Relaxation step: if we found a shorter path to v through u
                if (dist[u] != INF && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
    }

    // Reconstruct path if one was found
    if (found) {
        string current = endUser;
        while (!current.empty()) {
            path.push_front(current);
            if (parent.find(current) == parent.end() || parent[current].empty()) break;
            current = parent[current];
        }
    } else {
        cout << "Dijkstra: No path found between '" << startUser << "' and '" << endUser << "'." << endl;
    }

    return {finalDistance, path};
}

int main() {
    cout << "--- Social Network Simulation ---" << endl;
    SocialNetwork net;

    // Create the social network with users
    cout << "\n--- Adding Users ---" << endl;
    net.addUser("Alice");
    net.addUser("Bob");
    net.addUser("Charlie");
    net.addUser("David");
    net.addUser("Eve");
    net.addUser("Frank");
    net.addUser("Grace"); // User with no connections initially
    net.addUser("Heidi");

    // Build friendship connections
    cout << "\n--- Adding Friendships ---" << endl;
    net.addFriendship("Alice", "Bob");
    net.addFriendship("Alice", "Charlie");
    net.addFriendship("Bob", "David");
    net.addFriendship("Charlie", "David");
    net.addFriendship("Charlie", "Eve");
    net.addFriendship("David", "Eve");
    net.addFriendship("Eve", "Frank");
    net.addFriendship("Frank", "Heidi");

    // Display the network
    net.printGraph();

    // Test basic friend retrieval
    cout << "\n--- Testing: Get Friends ---" << endl;
    string userToQuery = "Charlie";
    set<string> charlieFriends = net.getFriends(userToQuery);
    cout << "'" << userToQuery << "'s friends: {";
    string separator = "";
    for (const string &f : charlieFriends) {
        cout << separator << "'" << f << "'";
        separator = ", ";
    }
    cout << "}" << endl;

    // Test user with no friends
    userToQuery = "Grace";
    set<string> graceFriends = net.getFriends(userToQuery);
    cout << "'" << userToQuery << "'s friends: {";
    separator = "";
    for (const string &f : graceFriends) {
        cout << separator << "'" << f << "'";
        separator = ", ";
    }
    cout << "}" << endl;

    // Test mutual friends functionality
    cout << "\n--- Testing: Mutual Friends ---" << endl;
    string userA = "Alice";
    string userB = "David";
    set<string> mutual = net.getMutualFriends(userA, userB);
    cout << "Mutual friends between '" << userA << "' and '" << userB << "': {";
    separator = "";
    for (const string &mf : mutual) {
        cout << separator << "'" << mf << "'";
        separator = ", ";
    }
    cout << "}" << endl; // Expected: Bob, Charlie

    // Test another mutual friends case
    userA = "Bob";
    userB = "Eve";
    mutual = net.getMutualFriends(userA, userB);
    cout << "Mutual friends between '" << userA << "' and '" << userB << "': {";
    separator = "";
    for (const string &mf : mutual) {
        cout << separator << "'" << mf << "'";
        separator = ", ";
    }
    cout << "}" << endl; // Expected: David

    // Test with non-existent user
    userA = "Alice";
    userB = "Nobody";
    mutual = net.getMutualFriends(userA, userB);

    // Test friend suggestion algorithm
    cout << "\n--- Testing: Suggest Friends ---" << endl;
    userToQuery = "Alice";
    vector<pair<string, int>> suggestions = net.suggestFriends(userToQuery);
    cout << "Friend suggestions for '" << userToQuery << "':" << endl;
    if (suggestions.empty()) {
        cout << "  None." << endl;
    } else {
        for (const auto &suggestion : suggestions) {
            cout << "  - '" << suggestion.first << "' (via " << suggestion.second << " connection(s))" << endl;
        }
    }

    // Test more friend suggestions
    userToQuery = "Bob";
    suggestions = net.suggestFriends(userToQuery);
    cout << "Friend suggestions for '" << userToQuery << "':" << endl;
    if (suggestions.empty()) {
        cout << "  None." << endl;
    } else {
        for (const auto &suggestion : suggestions) {
            cout << "  - '" << suggestion.first << "' (via " << suggestion.second << " connection(s))" << endl;
        }
    }

    // Test suggestions at network edge
    userToQuery = "Frank";
    suggestions = net.suggestFriends(userToQuery);
    cout << "Friend suggestions for '" << userToQuery << "':" << endl;
    if (suggestions.empty()) {
        cout << "  None." << endl;
    } else {
        for (const auto &suggestion : suggestions) {
            cout << "  - '" << suggestion.first << "' (via " << suggestion.second << " connection(s))" << endl;
        }
    }

    // Test BFS shortest path algorithm
    cout << "\n--- Testing: Shortest Path (BFS) ---" << endl;
    string start = "Alice";
    string end = "Eve";
    pair<int, list<string>> resultBFS = net.shortestPathBFS(start, end);

    cout << "Shortest path (BFS) from '" << start << "' to '" << end << "':" << endl;
    if (resultBFS.first != -1) {
        cout << "  Distance: " << resultBFS.first << " connections" << endl;
        cout << "  Path: ";
        separator = "";
        for (const string &node : resultBFS.second) {
            cout << separator << "'" << node << "'";
            separator = " -> ";
        }
        cout << endl; // Expected: Alice -> Charlie -> Eve
    }

    // Test longer BFS path
    start = "Bob";
    end = "Heidi";
    resultBFS = net.shortestPathBFS(start, end);
    cout << "Shortest path (BFS) from '" << start << "' to '" << end << "':" << endl;
    if (resultBFS.first != -1) {
        cout << "  Distance: " << resultBFS.first << " connections" << endl;
        cout << "  Path: ";
        separator = "";
        for (const string &node : resultBFS.second) {
            cout << separator << "'" << node << "'";
            separator = " -> ";
        }
        cout << endl; // Expected: Bob -> David -> Eve -> Frank -> Heidi
    }

    // Test BFS to disconnected user
    start = "Alice";
    end = "Grace";
    resultBFS = net.shortestPathBFS(start, end);
    cout << "Shortest path (BFS) from '" << start << "' to '" << end << "':" << endl;
    if (resultBFS.first != -1) {
        cout << "  Error: Path found unexpectedly!" << endl;
    }

    // Test BFS to non-existent user
    start = "Alice";
    end = "Nobody";
    resultBFS = net.shortestPathBFS(start, end);
    cout << "Shortest path (BFS) from '" << start << "' to '" << end << "':" << endl;
    if (resultBFS.first != -1) {
        cout << "  Error: Path found unexpectedly!" << endl;
    }

    // Test Dijkstra shortest path algorithm
    cout << "\n--- Testing: Shortest Path (Dijkstra - Bonus) ---" << endl;
    start = "Alice";
    end = "Heidi";
    pair<int, list<string>> resultDijkstra = net.shortestPathDijkstra(start, end);

    cout << "Shortest path (Dijkstra) from '" << start << "' to '" << end << "':" << endl;
    if (resultDijkstra.first != -1) {
        cout << "  Distance: " << resultDijkstra.first << " connections" << endl;
        cout << "  Path: ";
        separator = "";
        for (const string &node : resultDijkstra.second) {
            cout << separator << "'" << node << "'";
            separator = " -> ";
        }
        cout << endl; // Expected: Alice -> Charlie -> Eve -> Frank -> Heidi
    }

    // Test Dijkstra from disconnected user
    start = "Grace";
    end = "Alice";
    resultDijkstra = net.shortestPathDijkstra(start, end);
    cout << "Shortest path (Dijkstra) from '" << start << "' to '" << end << "':" << endl;
    if (resultDijkstra.first != -1) {
        cout << "  Error: Path found unexpectedly!" << endl;
    }

    // Test Dijkstra to non-existent user
    start = "Bob";
    end = "Nobody";
    resultDijkstra = net.shortestPathDijkstra(start, end);
    cout << "Shortest path (Dijkstra) from '" << start << "' to '" << end << "':" << endl;
    if (resultDijkstra.first != -1) {
        cout << "  Error: Path found unexpectedly!" << endl;
    }

    cout << "\n--- Testing Complete ---" << endl;
    return 0;
}