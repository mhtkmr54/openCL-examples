#include <GL/glut.h> 
 float blueness = 0.0; 
int bluedirec = 0;
 void Timer(int value)
{
   glutPostRedisplay();                 
   glutTimerFunc(35, Timer, 0);
}
void initGL() 
{

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
}
void idle()
{
    if(bluedirec ==0)
    blueness += 0.01;
    if (bluedirec == 1)
    blueness -=0.01;
    if(blueness >0.9)
    bluedirec = 1;
    if(blueness < 0.1)
    bluedirec = 0;
}
 
void display() 
{
   glClear(GL_COLOR_BUFFER_BIT);   
    glBegin(GL_POINTS);
      
    for(int i =-512;i<512;i++)
    {
        for(int j =-512;j<512;j++)
        {
            glColor3f((float)i/512,(float)j/512,blueness);
            glVertex2f((float)i/512,(float)j/512);
        }
    } 
    glEnd();
    
   glFlush();  
}
 

int main(int argc, char** argv) {
   glutInit(&argc, argv);          // Initialize GLUT
   glutCreateWindow("Vertex, Primitive & Color");  // Create window with the given title
   glutInitWindowSize(1024,1024);   // Set the window's initial width & height
   glutInitWindowPosition(50,50); // Position the window's initial top-left corner
   glutDisplayFunc(display ); 
   glutTimerFunc(0 , Timer , 0); 
   glutIdleFunc(idle);     // Register callback handler for window re-paint event
   initGL();                       // Our own OpenGL initialization
   glutMainLoop();                 // Enter the event-processing loop
   return 0;
}