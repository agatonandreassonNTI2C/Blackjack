int ace1 = 11;
int ace2 = 1;


void Random() {
  int r = random(13); // Slumpmässigt tal mellan 0 och 9
  if (r < 10) 
    return 10; // Sannolikhet 60% för att få 3
 
  else if (r == 1){
    return ace1;
    return ace2;
  }
  else
    return r; // Sannolikhet 20% för att få 1
}
  
