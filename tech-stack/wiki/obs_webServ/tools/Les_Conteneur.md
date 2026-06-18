En C++, le terme "container" fait généralement référence à une structure de données qui stocke et organise un ensemble d'éléments. Les conteneurs sont une partie importante de la bibliothèque standard C++ (STL, Standard Template Library) et offrent plusieurs types de conteneurs adaptés à différentes tâches. Les conteneurs en C++ sont utilisés pour stocker et manipuler des collections d'objets de manière efficace. Voici quelques exemples de conteneurs couramment utilisés en C++ :

1. **Vector (std::vector)** : Un vecteur est une séquence dynamique qui peut grandir ou rétrécir automatiquement. Il stocke ses éléments de manière contiguë en mémoire, ce qui permet un accès rapide aux éléments.
    
2. **List (std::list)** : Une liste doublement chaînée qui permet une insertion et une suppression rapides d'éléments à n'importe quel endroit de la liste. Cependant, l'accès aux éléments n'est pas aussi rapide qu'avec un vecteur.
    
3. **Deque (std::deque)** : Un deque (prononcé "deck") est une double-ended queue qui permet l'ajout et la suppression efficaces d'éléments en tête ou en queue de la structure. Il est similaire à un vector, mais il offre une efficacité accrue pour les opérations de déplacement en tête.
    
4. **Set (std::set)** : Un ensemble est une collection d'éléments uniques, triés par défaut. Il garantit que chaque élément est unique et offre une recherche efficace.
    
5. **Map (std::map)** : Une carte est une structure de données associant des clés à des valeurs. Chaque clé est unique, et les données sont généralement triées par clé.
    
6. **Unordered Set (std::unordered_set)** et **Unordered Map (std::unordered_map)** : Ce sont des versions de set et map qui n'ordonnent pas automatiquement leurs éléments. Ils utilisent une table de hachage pour une recherche rapide.
    
7. **Stack (std::stack)** : Une pile est une structure de données de type LIFO (Last-In, First-Out), souvent utilisée pour gérer des opérations de type pile.
    
8. **Queue (std::queue)** : Une file est une structure de données de type FIFO (First-In, First-Out), couramment utilisée pour gérer des opérations de type file d'attente.
    
9. **Priority Queue (std::priority_queue)** : Une file de priorité est utilisée pour stocker des éléments avec une priorité associée, où les éléments de plus grande priorité sont accessibles en premier.
    

Les conteneurs en C++ sont un moyen puissant d'organiser et de manipuler des données de manière efficace. Vous pouvez choisir le conteneur qui convient le mieux à votre tâche en fonction de vos besoins en termes de performance et de fonctionnalités.