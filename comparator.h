class Comparator {
public:
	Comparator() { }
    bool operator()(Packet &p1, Packet &p2) // p2 has highest prio than p1 if p2 is earlier than p1
    {
       if (p2.getLocation() < p1.getLocation()) return true;
	   if (p2.getLocation() == p1.getLocation()) return false;
	   if (p2.getLocation() > p1.getLocation()) return false;
       return false;
    }
};
