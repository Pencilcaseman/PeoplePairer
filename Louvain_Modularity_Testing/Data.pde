class Person {
  String name;
  int number; 
  int[] connections;
  
  Person(String nameData, int numberData, int[] connectionData) {
     name = nameData;
     number = numberData;
     connections = connectionData;
  }
  
  void show(x, y, r) {
     fill(255, 0, 0);
     stroke(0);
     strokeWeight(3);
     ellipse(x, y, r);
  }
}
