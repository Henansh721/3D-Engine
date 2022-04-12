#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<algorithm>
#include <SFML/Graphics.hpp>

using namespace std;

unsigned int w = sf::VideoMode::getDesktopMode().width;
unsigned int h = sf::VideoMode::getDesktopMode().height;



struct vertix
{
    float x = 0, y = 0, z = 0, w = 1;
};

struct triangle
{
    vertix p[3];
    float rbg = 7.0f;
    
};

struct shape
{
    vector<triangle> t;
    
};

bool comparevectors(triangle t1, triangle t2);

class worlds
{
public:
    shape wrld_cor3d, wrld_cor2d;
    float aspectratio = w / (float)h;
    float farplane = 1000.0f;
    float nearplane = 1.0f;
    float fov = 45.0f;
    float projmat[4][4] = { 0 };
    float rotmat[4][4] = { 0 };
    float cameramat[4][4] = { 0 };
    float theta = 45.0f;
    float fangle = 0;
    float change = 0;
    vertix camera = { 0,0,0,0 };
    vertix light = { 0,0,-1,0 };
    vertix targetvector = { 0,0,1,0 };
    vertix upvector = { 0,1,0,0 };
    vertix lookdir = { 0 };


public:

    worlds()
    {

        getcorrdinates("monkey.obj", wrld_cor3d);

        // Defining Projection matrix
        projmat[0][0] = 1 / (aspectratio * tan(fov / 2));
        projmat[1][1] = 1 / tan(fov / 2);
        projmat[2][2] = farplane / (farplane - nearplane);
        projmat[3][2] = -1.0f * (farplane * nearplane) / (farplane - nearplane);
        projmat[2][3] = 1.0f;

    }
public:

    void getcorrdinates(string filename, shape& wrld_cord3d)
    {

        ifstream file(filename);

        string line,s;
        vector<vertix> vectorpool;
        vector<float> a;


        if (!file.is_open())
            cout << "unable to open";

        while (!file.eof())
        {
            getline(file, line);

            if (line.length() > 1 && (line[0] == 'v' || line[0] == 'f'))
            {
                    for (int i = 0; i < line.length(); i++)
                        if (line[i] == ' ')
                        {
                            if (s != "v" && s != "f")
                                a.push_back(stof(s));
                            s.clear();
                        }
                        else
                            s += line[i];


                    if (s != "v" && s != "f")
                        a.push_back(stof(s) * 1.0f);

                    if (line[0] == 'v')
                        vectorpool.push_back({ a[0],a[1],a[2],0.0f });

                    if (line[0] == 'f')
                        wrld_cord3d.t.push_back({ vectorpool[a[0] - 1],vectorpool[a[1] - 1],vectorpool[a[2] - 1] });

                s.clear();
                a.clear();

            }

        }

        /*wrld_cor3d.t = {

            SOUTH
         { 0.0f, 0.0f, 0.0f,0.0f,     0.0f, 1.0f, 0.0f,0.0f,    1.0f, 1.0f, 0.0f,0.0f },
         { 0.0f, 0.0f, 0.0f,0.0f,    1.0f, 1.0f, 0.0f,0.0f,    1.0f, 0.0f, 0.0f,0.0f },

         EAST
        { 1.0f, 0.0f, 0.0f,0.0f,    1.0f, 1.0f, 0.0f,0.0f,    1.0f, 1.0f, 1.0f,0.0f },
        { 1.0f, 0.0f, 0.0f,0.0f,    1.0f, 1.0f, 1.0f,0.0f,    1.0f, 0.0f, 1.0f,0.0f },

        NORTH
        { 1.0f, 0.0f, 1.0f,0.0f,    1.0f, 1.0f, 1.0f,0.0f,    0.0f, 1.0f, 1.0f,0.0f },
        { 1.0f, 0.0f, 1.0f,0.0f,    0.0f, 1.0f, 1.0f,0.0f,    0.0f, 0.0f, 1.0f,0.0f },

         WEST
        { 0.0f, 0.0f, 1.0f,0.0f,    0.0f, 1.0f, 1.0f,0.0f,    0.0f, 1.0f, 0.0f,0.0f },
        { 0.0f, 0.0f, 1.0f,0.0f,    0.0f, 1.0f, 0.0f,0.0f,    0.0f, 0.0f, 0.0f,0.0f },

         TOP
        { 0.0f, 1.0f, 0.0f,0.0f,    0.0f, 1.0f, 1.0f,0.0f,    1.0f, 1.0f, 1.0f,0.0f },
        { 0.0f, 1.0f, 0.0f,0.0f,    1.0f, 1.0f, 1.0f,0.0f,    1.0f, 1.0f, 0.0f,0.0f },

         BOTTOM
        { 1.0f, 0.0f, 1.0f,0.0f,    0.0f, 0.0f, 1.0f,0.0f,    0.0f, 0.0f, 0.0f,0.0f },
        { 1.0f, 0.0f, 1.0f,0.0f,    0.0f, 0.0f, 0.0f,0.0f,    1.0f, 0.0f, 0.0f,0.0f },

        };*/
    }

    void matmultiplication(triangle input, triangle& output, float m[4][4])
    {
        float input_mat[3][4] = { 0 };
        float output_mat[3][4] = { 0 };

        for (int i = 0; i < 3; i++)
        {
            input_mat[i][0] = input.p[i].x;
            input_mat[i][1] = input.p[i].y;
            input_mat[i][2] = input.p[i].z;
            input_mat[i][3] = input.p[i].w;
        }

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 4; j++)
            {
                output_mat[i][j] = 0;
                for (int k = 0; k < 4; k++)
                    output_mat[i][j] += input_mat[i][k] * m[k][j];
            }

        for (int i = 0; i < 3; i++)
        {
            output.p[i].x = output_mat[i][0];
            output.p[i].y = output_mat[i][1];
            output.p[i].z = output_mat[i][2];
            output.p[i].w = output_mat[i][3];
        }
    }

    void getprojection(triangle input, triangle& output)
    {
        matmultiplication(input, output, projmat);

        for (int i = 0; i < 3; i++)
            if (output.p[i].z != 0.0f)
            {
                output.p[i].x /= output.p[i].z;
                output.p[i].y /= output.p[i].z;

            }
    }

    vertix getvertix(vertix a, vertix b, vertix normal, float dorigin)
    {
        vertix parametric;
        float alpha;

        alpha = ((normal.x * a.x) + (normal.y * a.y) + (normal.z * a.z) - dorigin) / ((normal.x * (a.x - b.x)) + (normal.y * (a.y - b.y)) + (normal.z * (a.z - b.z)));

        parametric.x = alpha * (b.x - a.x) + a.x;
        parametric.y = alpha * (b.y - a.y) + a.y;
        parametric.z = alpha * (b.z - a.z) + a.z;

        return parametric;
    }

    int getclipping(vertix normal, triangle input, vector<triangle>& output, float dorigin)
    {
        int count_outside = 0;
        int count_inside = 0;

        vector<vertix> outside, inside;
        triangle tri1, tri2;

        for (int i = 0; i < 3; i++)
            if ((normal.x * input.p[i].x) + (normal.y * input.p[i].y) + (normal.z * input.p[i].z) + dorigin < 0)
            {
                outside.push_back(input.p[i]);
                count_outside++;
            }
            else
            {
                inside.push_back(input.p[i]);
                count_inside++;
            }

        if (count_outside == 1 && count_inside == 2)
        {
            tri1.p[0] = inside[0];
            tri1.p[1] = inside[1];
            tri1.p[2] = getvertix(inside[1], outside[0], normal, dorigin);

            tri2.p[0] = inside[0];
            tri2.p[1] = tri1.p[2];
            tri2.p[2] = getvertix(inside[0], outside[0], normal, dorigin);

            output.push_back(tri1);
            output.push_back(tri2);

            return 2;

        }
        else
            if (count_outside == 2 && count_inside == 1)
            {
                tri1.p[0] = inside[0];
                tri1.p[1] = getvertix(inside[0], outside[0], normal, dorigin);
                tri1.p[2] = getvertix(inside[0], outside[1], normal, dorigin);

                output.push_back(tri1);

                return 1;
            }
            else
                if (count_outside == 0 && count_inside == 3)
                {
                    tri1.p[0] = inside[0];
                    tri1.p[1] = inside[1];
                    tri1.p[2] = inside[2];

                    output.push_back(tri1);

                    return 1;
                }
                else
                    return 0;

    }

    void transformation(sf::Clock time)
    {
        triangle transformed,rottriangle,viewedtriangle;
        vector<triangle> clipped;
        vertix normal;
        int numclipped = 0;
        wrld_cor2d.t.clear();

        //theta = time.getElapsedTime().asSeconds() * 1.5f;


        //rotmat[0][0] = cos(theta);
        //rotmat[0][1] = (sin(theta / 2.0f) + sin((3.0f * theta) / 2.0f)) / 2;
        //rotmat[0][2] = (cos(theta / 2.0f) - cos((3.0f * theta) / 2.0f)) / 2;
        //rotmat[1][0] = -1.0f * sin(theta);
        //rotmat[1][1] = (cos(theta / 2.0f) + cos((3.0f * theta) / 2.0f)) / 2;
        //rotmat[1][2] = (-sin(theta / 2.0f) + sin((3.0f * theta) / 2.0f)) / 2;
        //rotmat[2][1] = -1.0f * sin(theta/2.0f);
        //rotmat[2][2] = cos(theta/2.0f);
        //rotmat[3][3] = 1.0f;

        rotmat[0][0] = cos(90 -fangle);
        rotmat[0][2] = sin(90 -fangle);
        rotmat[1][1] = 1.0f;
        rotmat[2][0] = -sin(90 -fangle);
        rotmat[2][2] = cos(90 -fangle);
        rotmat[3][3] = 1.0f;

                      
        for (auto &triangles : wrld_cor3d.t)
        {

            clipped.clear();
            numclipped = 0;

  
            matmultiplication(triangles, rottriangle, rotmat);

           // rottriangle = triangles;
   
            rottriangle.p[0].z += 8.0f;
            rottriangle.p[1].z += 8.0f;
            rottriangle.p[2].z += 8.0f;

            rottriangle.p[0].y -= 0.5f;
            rottriangle.p[1].y -= 0.5f;
            rottriangle.p[2].y -= 0.5f;

            rottriangle.p[0].z -= camera.z;
            rottriangle.p[1].z -= camera.z;
            rottriangle.p[2].z -= camera.z;

            rottriangle.p[0].x -= camera.x;
            rottriangle.p[1].x -= camera.x;
            rottriangle.p[2].x -= camera.x;

            rottriangle.p[0].y -= camera.y;
            rottriangle.p[1].y -= camera.y;
            rottriangle.p[2].y -= camera.y;
                
            //numclipped = getclipping({ 0.0f,0.0f,1.0f }, rottriangle, clipped, 1.0f);


            //for (int i = 0; i < numclipped; i++)
            //{
                //getnormal(clipped[i], normal);
            getnormal(rottriangle, normal);

               // if ((normal.x * (clipped[i].p[0].x - camera.x) + normal.y * (clipped[i].p[0].y - camera.y) + normal.z * (clipped[i].p[0].z - camera.z)) < 0.0f)
                if ((normal.x * (rottriangle.p[0].x - camera.x) + normal.y * (rottriangle.p[0].y - camera.y) + normal.z * (rottriangle.p[0].z - camera.z)) < 0.0f)
                {
                    //normalize(light); will light vector change ?


                    getprojection(rottriangle, transformed);

                    for (int i = 0; i < 3; i++)
                    {
                        transformed.p[i].x += 1.0f;
                        transformed.p[i].y += 1.0f;
                        transformed.p[i].x *= 0.5f * (float)w;
                        transformed.p[i].y *= 0.5f * (float)h;

                    }

                    transformed.rbg = normal.x * light.x + normal.y * light.y + normal.z * light.z;
                    wrld_cor2d.t.push_back(transformed);



               }
            }
       //}

        sort(wrld_cor2d.t.begin(), wrld_cor2d.t.end(), comparevectors);
      
    }

    void getnormal(triangle input, vertix &normal)
    {
        vertix a, b;

        a.x = input.p[1].x - input.p[0].x;
        a.y = input.p[1].y - input.p[0].y;
        a.z = input.p[1].z - input.p[0].z;

        b.x = input.p[2].x - input.p[0].x;
        b.y = input.p[2].y - input.p[0].y;
        b.z = input.p[2].z - input.p[0].z;

        normal.x = a.y * b.z - b.y * a.z;
        normal.y = b.x * a.z - a.x * b.z;
        normal.z = a.x * b.y - b.x * a.y;

        normalize(normal);

    }

    void normalize(vertix &vector)
    {
        float mag = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

        vector.x /= mag;
        vector.y /= mag;
        vector.z /= mag;
    }


};

class game
{

private:
    sf::RenderWindow window;
    worlds world;

public:
    game();
    void run();

private:
    void processevents(sf::Clock time);
    void update(sf::Clock time);
    void render();


};

int main()
{
    game game;
    game.run();

    return 0;
}

game::game() : window(sf::VideoMode(w, h), "Project 3D", sf::Style::Fullscreen)
{

}


void game::run()
{
    sf::Clock time;

    while (window.isOpen())
    {
        processevents(time);
        update(time);
        render();
    }

}

void game::processevents(sf::Clock time)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    float speed = 0.001f;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        world.camera.z -= 100.0f * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        world.camera.z += 100.0f * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        world.camera.x += 100.0f * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        world.camera.x -= 100.0f * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        world.camera.y += 100.0f * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        world.camera.y -= 100.0f * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        world.fangle += 10.0f * speed;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        world.fangle -= 10.0f * speed;
    }

}

void game::update(sf::Clock time)
{
    world.transformation(time);
}

void game::render()
{
    
    window.clear(sf::Color::Black);

        sf::CircleShape vertix(2.0f);
    vertix.setPosition(sf::Vector2f(0+1*0.5*w, 0.5*h));
    vertix.setFillColor(sf::Color::Blue);

    
    for (const auto& tp : world.wrld_cor2d.t)
    {
        sf::ConvexShape triangle;
        triangle.setPointCount(3);
        triangle.setFillColor(sf::Color::Black);
        //triangle.setFillColor(sf::Color(250 * tp.rbg, 250 * tp.rbg, 250 * tp.rbg));
        triangle.setOutlineColor(sf::Color::White);

        triangle.setOutlineThickness(0.5);

        //for (int i = 0; i < 3; i++)
        //{
        //    sf::CircleShape vertix(2.0f);
        //    vertix.setPosition(sf::Vector2f(tp.p[i].x, tp.p[i].y));
        //    vertix.setFillColor(sf::Color::Red);
        //    window.draw(vertix);
        //}

     
        for (int i = 0; i < 3; i++)
            triangle.setPoint(i, sf::Vector2f( tp.p[i].x,  tp.p[i].y));

        window.draw(triangle);

    }
    window.draw(vertix);
    window.display();
    
}

bool comparevectors(triangle t1, triangle t2)
{
    float a = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
    float b = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;

    return a > b;
}