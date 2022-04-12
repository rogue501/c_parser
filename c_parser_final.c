/*********************************************
 * Id: rogue
 *
 * Compile: gcc -Wall
 * Run: ./a.out [input.txt]
 *
 * 
 *********************************************/

#define MAXTOKEN 256
#define MAXSTRING 258
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

// Scans file line by line
void Scanner(char *line, int length, int* mFlag);

// Lexes Tokens from line
int MakeToken(char *line, int start, int length);

// Creates String from line
int MakeString(char *line, int start);

// Creates Char litteral from line
int MakeChar(char *line, int start);

// Creates comment from line
int MakeMultiComment(char *line, int start, int* mFlag, int length);

// Tests if charecter is a quote mark
bool IsQuote(char c);

// Tests if charecter is apostrophe
bool IsApos(char c);

// Tests if string contains keyword
bool IsKeyword(char *token);

// Tests if token is a numeric literal
bool IsNumlit(char *token);

// Tests if token is an Identifier
bool IsIdent(char *token);

int main( int argc, char *argv[] ) {
   if (argc < 2){
      printf("Must enter vallid path for input file\n");
      printf("%s /y/shared/Engineering/cs-drbc/assignments/cs210/w01_in1.txt\n",                                    argv[0]);
      return 1;
   }
   //printf("You entered the file name: %s\n", argv[1]);

   FILE *fp;
   char *line = NULL;
   line = (char*)malloc(sizeof(char)*MAXSTRING);
   size_t len = 0;
   size_t read;

   // Open file, check to make sure file open.
   fp = fopen(argv[1], "r");
   if(fp == NULL){
      printf("ERROR: Unable to open file: %s\n", argv[1]);
      return 2;
   }

   int mFlag = 0;
   // Read in file, scan, tokenize
   while ((read = getline( &line, &len, fp)) != -1){
      //printf("%ld ", read);
      //printf("%s\n", line);

      int i = 0;
      Scanner(line, read, &mFlag);
      for(i = 0 ; i < len ; i++){
         line[i] = '\0';
      }
   }

   // Close file, free memory
   fclose(fp);
   if(line){
      free((void*)line);
   }

   return 0;
}

// Identifies comments, chars, strings, tokens in line
void Scanner(char *line, int length, int* mFlag){
   int i;
   int tokenStart = 0;

   // Scan lines
   for(i = 0; i < length; i++){
      top:

      // Multiline Comment mode flagged
      if(*mFlag == 1){
         i=MakeMultiComment(line, i, mFlag, length);
         tokenStart = i;        
      }

      // Start of multiline comment
      else if(line[i] == '/' && line[i+1] == '*'){
         i = MakeMultiComment(line, i, mFlag, length);
         tokenStart = i;
         if (i < length){
            goto top;
         }
      }

      // Identify character literals
      else if (IsApos(line[i])){
         i = MakeChar(line, i);
         tokenStart = i;
         if (i < length){
            goto top;
         }
      }
   
      // Identify strings
      else if (IsQuote(line[i])){
         i = MakeString(line, i);
         tokenStart = i;
         if (i < length){
            goto top;
         }
      }

      // Identify tokens
      else if(isspace(line[i])){
         i = MakeToken(line, tokenStart, length);
         tokenStart = i;
         if (i < length){
            goto top;
         }
      }

   }
}

// Creates comment from line
int MakeMultiComment(char *line, int start, int* mFlag, int length){
   // Enter Multi line comment mode
   *mFlag = 1;

   char charString[MAXSTRING];
   int i;
   int end;

   // Identify end mark
   for (i = start+1; i < length; i++){
      if (line[i] == '*' && line[i+1] == '/'){
         *mFlag = 0;
         end = i+1;
         break;
      }
      else if (i == length-1){
         end = i;
      }
   }

   // Check if end comment is start of line
   i = 0;
   if(line[i] == '*' && line[i+1]== '/' ){
      end = i+1;
      *mFlag = 0;
   }

   // Sperate comment from line
   strncpy( charString, &line[start], end-start+1);
   charString[end-start+1] = '\0';
   if (end == length-1){
      charString[end-start] = '\0';
   }

   // Print string, return end of quote
   printf("Comment: %s\n", charString);
   return end+1;
}

// Creates Char from line
int MakeChar(char *line, int start){

   char charString[MAXSTRING];
   int i, j;
   int end;
   // Clear token buffer
   for(i = 0 ; i < MAXSTRING ; i++){
      charString[i] = '\0';
   }

   // Identify end mark
   for (i = start+1; i < MAXSTRING; i++){
      if (IsApos(line[i])){
         end = i;
         break;
      }
   }

   // Sperate string from line
   for (i = start, j = 0; i < end+1; i++, j++){
      charString[j] = line[i];
   }
   charString[i+1] = '\0';

   // Print string, return end of quote
   printf("Char: %s\n", charString);
   return end+1;

}

// Creates String from line
int MakeString(char *line, int start){

   char charString[MAXSTRING];
   int i, j;
   int end;
   // Clear token buffer
   for(i = 0 ; i < MAXSTRING ; i++){
      charString[i] = '\0';
   }

   // Identify end quote
   for (i = start+1; i < MAXSTRING; i++){
      if (IsQuote(line[i])){
         end = i;
         break;
      }
   }

   // Sperate string from line
   for (i = start, j = 0; i < end+1; i++, j++){
      charString[j] = line[i];
   }
   charString[i+1] = '\0';

   // Print string, return end of quote
   printf("String: %s\n", charString);
   return end+1;

}

// Removes spaces, assigns meaning to string.
int MakeToken(char *line, int start, int length){
   //printf("MakeTOken Line: %s\n", line);
   //printf("MakeTOken start: %i\n", start);
   // Find true end
   int z = start;
   int end = length;
   for(; z < length; z++){
      if (IsApos(line[z]) || IsQuote(line[z])){
         end = z;
         break;
      }
      else if (line[z]== '/' && line[z+1] == '*'){
        end = z;
        break;
      }
   }
   //printf("MakeToken end: %i\n", end);


   int i = start;
   

   // Process line
   while( i < end){

      if (isspace(line[i])){
        i++;
      }

      else if (isalpha(line[i]) || isdigit(line[i]) || line[i]=='_' || line[i]=='?'|| line[i]=='^'|| line[i]=='~' || line[i]=='#'|| (line[i]== '.' && line[i+1] != '.')||(line[i]== '!' && line[i+1] != '=')){
         int z = 0;
         char temp[end-start];
         while (isalpha(line[i]) || isdigit(line[i]) || line[i]=='_' || line[i]== '.'|| line[i]=='?'|| line[i]=='^'|| line[i]=='~' || line[i]=='#'|| line[i] == '!'){
              temp[z]= line[i];
              i++;
              z++;
         }
         temp[z]='\0';
         if(!IsKeyword(temp)){
            if(!IsNumlit(temp)){
               if(!IsIdent(temp)){
                  printf("Token: %s\n", temp);
               }   
            }
         }  
      }

      else if (line[i] == '<'){
         if(line[i+1]== '<'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else if(line[i+1]== '='){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else if(line[i+1]== '>'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Operator: %c\n", line[i]);
            i++;
         }
      }

      else if (line[i] == '>'){
         if(line[i+1]== '>'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else if(line[i+1]== '='){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Operator: %c\n", line[i]);
            i++;
         }
      }

      else if (line[i] == '{'){
         if(line[i+1]== ':'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Operator: %c\n", line[i]);
            i++;
         }     
      }
      else if (line[i] == '}'){
         if(line[i+1]== ':'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Operator: %c\n", line[i]);
            i++;
         }     
      }

      else if (line[i] == ':'){
         if(line[i+1]== '='){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Operator: %c\n", line[i]);
            i++;
         }     
      }

      else if (line[i] == '*'){
         if(line[i+1]== '*'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Operator: %c\n", line[i]);
            i++;
         }     
      }

      else if (line[i] == '='){
         if(line[i+1]== '>'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Operator: %c\n", line[i]);
            i++;
         }     
      }

      else if (line[i] == '.'){
         if(line[i+1]== '.'){
            printf("Operator: %c%c\n", line[i], line[i+1]);
            i+=2;
         }
         else {
            printf("Token: %c\n", line[i]);
            i++;
         } 
      }

      else if (line[i] == '!'&& line[i+1]=='='){
         printf("Operator: %c%c\n", line[i], line[i+1]);     
         i+=2;
      }

      else if (line[i] == '+'){
         printf("Operator: %c\n", line[i]);   
         i++; 
      }

      else if (line[i] == '-'){
         printf("Operator: %c\n", line[i]);
         i++;    
      }

      else if (line[i] == '/'){
         printf("Operator: %c\n", line[i]); 
         i++;   
      }

      else if (line[i] == '|'){
         printf("Operator: %c\n", line[i]);
         i++;    
      }

      else if (line[i] == '&'){
         printf("Operator: %c\n", line[i]);
         i++;   
      }

      else if (line[i] == ';'){
         printf("Operator: %c\n", line[i]);
         i++;     
      }

      else if (line[i] == ','){
         printf("Operator: %c\n", line[i]);
         i++;      
      }

      else if (line[i] == '$'){
         printf("Operator: %c\n", line[i]);
         i++;     
      }

      else if (line[i] == '@'){
         printf("Operator: %c\n", line[i]);
         i++;      
      }

      else if (line[i] == '('){
         printf("Operator: %c\n", line[i]); 
         i++;     
      }
      else if (line[i] == ')'){
         printf("Operator: %c\n", line[i]);
         i++;     
      }

      else if (line[i] == '['){
         printf("Operator: %c\n", line[i]);
         i++;      
      }
      else if (line[i] == ']'){
         printf("Operator: %c\n", line[i]);
         i++;      
      }

      else{
         i++;
      }
   }

   return end;

}

// Tests if charecter is a quote mark
bool IsQuote(char c){
   if (c == '"'){
      return true;
   }
   return false;
}

// Tests if charecter is apostropher
bool IsApos(char c){
   if (c == 39){
      return true;
   }
   return false;
}

// Tests if string contains keyword
bool IsKeyword(char *token){

   // Array of Keywords to test against
   char *keyWords[35] = {"accessor", "and", 
   "array", "bool", "case", "character", 
   "constant", "elsif", "else", "end", 
   "exit", "float", "func", "integer", 
   "ifc", "if", "in", "is", "mutator", 
   "natural", "null", "of", "or", "others", 
   "out", "pkg", "positive", "proc", 
   "ptr", "range", "subtype", "then",
   "type", "when", "while"};
   
   // Is token a keyword?
   int i = 0;
   while (i<35){
      if (strcmp( token, keyWords[i]) == 0){
         printf("Keyword: %s\n", token);
         return true;
      }
      else {
         i++;
      }
   } 
  return false;
}

// Tests if token is a numeric literal
bool IsNumlit(char *token){
  
   int length = strlen(token);
   int i = 0;

   while (i < length) {
      if (!isdigit(token[i])){
         if (token[i] != 'A' || token[i] != 'B' || token[i] != 'C' || token[i] != 'D' || token[i] != 'E' || token[i] != 'F' || token[i] != 'a' || token[i] != 'b' || token[i] != 'c' || token[i] != 'd' || token[i] != 'e' || token[i] != 'f' ){
            return false;
         }
      }
   }
   printf("Numeric: %s\n", token);
  return true;
}

// Tests if token is an Identifier
bool IsIdent(char *token){
  
   int length = strlen(token);
   int i = 0;

   while (i < length) {
      if (!isdigit(token[i])){
         if (!isalpha(token[i])){
            if (token[i] != '_'){
               return false;
            }
         }
      }
   }
   printf("Identifier: %s\n", token);
  return true;
}
