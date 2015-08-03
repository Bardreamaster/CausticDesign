#include "global.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

// Qt
#include <QtGui>
#include <QDialog>
#include <QActionGroup>
#include <QFileDialog>
#include <QInputDialog>
#include <QClipboard>

#include "rendering.h"
#include "SurfaceModel.h"
#include <iostream>

Renderer::Renderer(int framesPerSecond, QWidget *parent , char *name):QGLWidget(parent){
    setWindowTitle(QString::fromUtf8(name));
      if(framesPerSecond == 0)
          t_Timer = NULL;
      else
      {
          int seconde = 1000; // 1 seconde = 1000 ms
          int timerInterval = seconde / framesPerSecond;
          t_Timer = new QTimer(this);
          connect(t_Timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
          t_Timer->start( timerInterval );
      }
}

void Renderer::printVersion(){
    std::cout<<"gpu"<<glGetString(GL_RENDERER)<<"opengl version:"<< glGetString(GL_VERSION) <<std::endl;
}

void Renderer::keyPressEvent(QKeyEvent *keyEvent){
    switch(keyEvent->key())
    {
        case Qt::Key_Escape:
            close();
            break;
    }
}

void Renderer::timeOutSlot(){}



/*Model Rendering*/

ModelRendering::ModelRendering(QWidget *parent):Renderer(60,parent,"Model Rendering")
{
    setModel();
    model.meshes[0].generateTriangles();

}

void ModelRendering::initializeGL(){
    initializeGLFunctions();
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

}


void ModelRendering::resizeGL(int width, int height){
    if(height == 0)
        height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
}
void ModelRendering::paintGL(){
    //glCullFace(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    Mesh meshToDraw = model.meshes[0];
    Vertex v;
    int i=0;

    glTranslatef(-1.5f, 0.0f, -6.0f);
    while (i<36){
        glBegin(GL_TRIANGLES);                                 //la ligne coupe le triangle et est partiellement cachée
            glColor3f(0.0f,0.0f,1.0f);
            glVertex3f(meshToDraw.vertices[i].Position.x,meshToDraw.vertices[i].Position.y,meshToDraw.vertices[i].Position.z);
            glColor3f(0.0f,0.0f,1.0f);
            glVertex3f(meshToDraw.vertices[i+1].Position.x,meshToDraw.vertices[i+1].Position.y,meshToDraw.vertices[i+1].Position.z);
            glColor3f(1.0f,0.0f,0.0f);
            glVertex3f(meshToDraw.vertices[i+2].Position.x,meshToDraw.vertices[i+2].Position.y,meshToDraw.vertices[i+2].Position.z);
        glEnd();
        glFlush();
        i+=3;
     }

//    Mesh meshToDraw = model.meshes[0];
//    glGenVertexArrays(1, &meshToDraw.VAO);
//    glGenBuffers(1, &meshToDraw.VBO);
//    glGenBuffers(1, &meshToDraw.EBO);

//    glBindVertexArray(meshToDraw.VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, meshToDraw.VBO);

//    glBufferData(GL_ARRAY_BUFFER, meshToDraw.vertices.size() * sizeof(Vertex),
//                 &meshToDraw.vertices[0], GL_STATIC_DRAW);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshToDraw.EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshToDraw.indices.size() * sizeof(GLuint),
//                 &meshToDraw.indices[0], GL_STATIC_DRAW);

//    // Vertex Positions
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//                         (GLvoid*)0);
//    // Vertex Normals
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//                         (GLvoid*)offsetof(Vertex, Normal));
//    // Vertex Texture Coords
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
//                         (GLvoid*)offsetof(Vertex, TexCoords));

//    glBindVertexArray(0);

//    glBindVertexArray(meshToDraw.VAO);
//    glDrawElements(GL_TRIANGLES, meshToDraw.indices.size(), GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);


//    glBegin(GL_TRIANGLES);
//        glVertex3f(0.0f, 1.0f, 0.0f);
//        glVertex3f(-1.0f, -1.0f, 0.0f);
//        glVertex3f(1.0f, -1.0f, 0.0f);
//    glEnd();

//    glTranslatef(3.0f, 0.0f, -6.0f);


}



void ModelRendering::setModel(){
    QString modelToLoad = QFileDialog::getOpenFileName(this, tr("Open 3D model to load"), ".");
    if(modelToLoad.isEmpty()) return;
    model.loadModel(modelToLoad.toStdString());
    update();
    model.printAllVertices();
    initializeGLFunctions();
    //glewInit(); //not any more necessary with qglfunction

}
void ModelRendering::setUpMesh(Mesh meshToDraw){
    makeCurrent();
    std::cout << "setupMesh function" << std::endl;
    glGenVertexArrays(1, &meshToDraw.VAO);
    glGenBuffers(1, &meshToDraw.VBO);
    glGenBuffers(1, &meshToDraw.EBO);

    glBindVertexArray(meshToDraw.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, meshToDraw.VBO);

    glBufferData(GL_ARRAY_BUFFER, meshToDraw.vertices.size() * sizeof(Vertex),
                 &meshToDraw.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshToDraw.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshToDraw.indices.size() * sizeof(GLuint),
                 &meshToDraw.indices[0], GL_STATIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (GLvoid*)0);
    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (GLvoid*)offsetof(Vertex, Normal));
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                         (GLvoid*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

}

void ModelRendering::drawMesh(Mesh meshToDraw){
    makeCurrent();
    glBindVertexArray(meshToDraw.VAO);
    glDrawElements(GL_TRIANGLES, meshToDraw.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
