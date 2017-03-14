

#include "Element2D.h"

#include "../Log.h"
#include "../common/texture.hpp"

#include "../OpenGLContainer.h"

using namespace std;
using namespace tinyxml2;
using namespace glm;

Instance2D::Instance2D()
	:type("Empty"),marked_for_cleanup(true),hidden(true),layer(-1),transform(),size_x(1.0f),size_y(1.0f),vertex_count(0)
	,image_name("Instance default constructed, no image name available")
	,grey(false),colour_r(0),colour_g(0),colour_b(0),transparency(0),offset()
{
};


Instance2D::Instance2D(std::string image_name_, Transform2D transf)
	:type("Image"),marked_for_cleanup(false), hidden(false), layer(0), transform(transf), size_x(1.0f), size_y(1.0f), vertex_count(0)
	, image_name(image_name_)
	, grey(false), colour_r(1.0f), colour_g(1.0f), colour_b(1.0f), transparency(0.0f), offset()
{

	MakeSimpleRectangleMesh();
};



Instance2D::Instance2D(int mode,std::string text, Transform2D transf, OffsetElement offset_, float max_box_size_x)
	:type("Text"),marked_for_cleanup(false), hidden(false), layer(0), transform(transf), size_x(1.0f), size_y(1.0f), vertex_count(0)
	, image_name("../Data/2D/font.png")
	, grey(false), colour_r(1.0f), colour_g(1.0f), colour_b(1.0f), transparency(0.0f), offset(offset_)
{
	if (text == "") {
		Log("Debug", "Building form empty text");
		hidden = true;
		return;
	}

	if (mode == 0) { //Build from text
		AttemptToLoadImage();
		const float size = 1.0f;//Changing the size here should never be needed because transformation matrices do all the work
		int length = text.size();

		//float vertexPosition = 0.0f;

		vector<glm::vec3> vertices; //for VBO
		vector<glm::vec2> uvs; //for VBO
		//Constant for now
		float relativeWidthOfThisChar = 0.6f;
		float widthOfThisChar = size * relativeWidthOfThisChar;


		vector<vector<glm::vec3>> words; //Cache current word (for linebreaks)
		vector<float> word_lengths;



		//build list of words, separating at whitespaces
		auto c_now = text.cbegin();
		auto c_next = find(text.cbegin(),text.cend(),' ');
		while(c_now!=text.cend()) {
			vector<glm::vec3> current_word;
			float current_pos_x = 0;

			for (auto it = c_now; it != c_next; it++) {
				char character = *it;
				auto vs = letter_vertices(widthOfThisChar,current_pos_x);
				current_word.insert(current_word.end(), vs.begin(), vs.end());
				//vertex_count = vertex_count + 6;
				current_pos_x += widthOfThisChar;

				//Unrelated: Add texture for this one TODO: Check if every letter gets added exactly once, otherwise it might look messy!
				auto uvs_a = letter_uvs(widthOfThisChar, character);
				uvs.insert(uvs.end(), uvs_a.begin(), uvs_a.end());
			}

			words.push_back(current_word);
			word_lengths.push_back(current_pos_x);

			if (c_next == text.cend()) {
				break;
			}
			c_now = c_next++;//Dont use ' ' twice
			c_next = find(c_next, text.cend(), ' ');
		}


		//Simplest algorithm, just add all words together
		size_x = 0;
		if (max_box_size_x <= 0) {

			//Iterate over all words
			for (unsigned int i = 0; i < words.size(); i++) {

				//Iterate over the i-th words vertices
				for (auto it = words[i].begin(); it != words[i].end(); it++) {
					auto a = *it;
					a.x += size_x; //Shift the words letters into position
					vertices.push_back(a); // *it);//Add letter-vertex to list 
				}

				size_x += word_lengths[i];

			}
			size_y = size;
		}

		else { //max_box_size > 0
			size_x = max_box_size_x;
			int lines = 1;
			float current_pos_x = 0.0f;

			for (unsigned int i = 0; i < words.size(); i++) {
				//If the current word is too long put it in the next row
				if (current_pos_x + word_lengths[i] > max_box_size_x ) {
					lines++;
					current_pos_x = 0.0f;
				}

				//Iterate over the i-th words vertices
				for (auto it = words[i].begin(); it != words[i].end(); it++) {
					auto a = *it;
					a.x += current_pos_x; //Shift the words letters into position
					a.y -= size * (lines - 1);
					vertices.push_back(a); // *it);//Add letter-vertex to list 
				}

				current_pos_x += word_lengths[i];
			}


			size_y = size * lines;
		}


		if (vertices.size() != uvs.size()) {
			Log("Error", "Mismatch between lengths of UVs and Vertices produced");
		}

		vertex_count = vertices.size();
		MakeModel(vertices, uvs);
	}//build from text
}





Instance2D::Instance2D(tinyxml2::XMLElement * xml_root) 
	:type("Image"),marked_for_cleanup(false), hidden(false), layer(0), transform(), size_x(1.0f), size_y(1.0f), vertex_count(0)
	, image_name("")
	, grey(false), colour_r(1.0f), colour_g(1.0f), colour_b(1.0f), transparency(0.0f), offset()
{

	{ //Look up an image name
		XMLElement * xml_child = xml_root->FirstChildElement("ImageFile");
		if (xml_child != nullptr) {
			image_name = xml_child->GetText();
		}
	}


	{ 
		XMLElement * xml_child = xml_root->FirstChildElement("Offset");
		if (xml_child != nullptr) {
			offset = OffsetElement(xml_child);
		}
	}


	MakeSimpleRectangleMesh();
};


bool Instance2D::MakeModel(std::vector<glm::vec3> T_vertices, std::vector<glm::vec2> T_uvs) {
	vertex_count = T_vertices.size(); //

	//Make a Vao
	glGenVertexArrays(1, &VertexArrayID);//Create 1 Vao and store its id in the GLuint
	glBindVertexArray(VertexArrayID);

	//Store in Vao
	GLuint PerInstance_vertexbuffer;
	glGenBuffers(1, &PerInstance_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PerInstance_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, T_vertices.size() * sizeof(glm::vec3), &T_vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	GLuint PerInstance_uvbuffer;
	glGenBuffers(1, &PerInstance_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PerInstance_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, T_uvs.size() * sizeof(glm::vec2), &T_uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	glBindVertexArray(0);
	//Important to avoid memory leak
	glDeleteBuffers(1, &PerInstance_vertexbuffer);
	glDeleteBuffers(1, &PerInstance_uvbuffer);

	//cout<<"Attempting to load texture with name"<<textureFileName<<endl;
	//texture = loadDDS("../3D/uvmap.DDS");

	//cout << "Sucessfully loaded!" << endl;
	return true;
};



bool Instance2D::AttemptToLoadImage() {
	//cout << "attempting to load texture:" << image_name << endl;

	//Log("Data","Loading 3d element texture ["+renderDefinition.textureFileName+"] on the fly");
	image = loadPNG_stb(image_name.c_str());
	if (image == 0) {
		Log("Error(Data)", "Failure loading texture: " + image_name);
		return false;
	}

	return true;
}

vector<glm::vec3> Instance2D::letter_vertices(float widthOfThisChar, float rel_x)
{
	vector<glm::vec3> res;
	glm::vec3 vertex_up_left = glm::vec3(rel_x, 1.0f / 2, 0);
	glm::vec3 vertex_up_right = glm::vec3(rel_x + widthOfThisChar, 1.0f / 2, 0);
	glm::vec3 vertex_down_right = glm::vec3(rel_x + widthOfThisChar, -1.0f / 2, 0);
	glm::vec3 vertex_down_left = glm::vec3(rel_x, -1.0f / 2, 0);

	res.push_back(vertex_up_left);
	res.push_back(vertex_down_left);
	res.push_back(vertex_up_right);
	res.push_back(vertex_down_right);
	res.push_back(vertex_up_right);
	res.push_back(vertex_down_left);

	return res;
}
std::vector<glm::vec2> Instance2D::letter_uvs(float widthOfThisChar, char letter)
{
	vector<glm::vec2> uvs;
	float uv_x = (letter % 16) / 16.0f;
	float uv_y = (letter / 16) / 16.0f;

	float offset_x = (1 - widthOfThisChar) / 2.0f / 16.0f;
	float offset_y = -0.002f;

	glm::vec2 uv_up_left = glm::vec2(uv_x + offset_x, uv_y - offset_y);
	glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f - offset_x, uv_y - offset_y);
	glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f - offset_x, (uv_y + 1.0f / 16.0f) + offset_y);
	glm::vec2 uv_down_left = glm::vec2(uv_x + offset_x, (uv_y + 1.0f / 16.0f) + offset_y);


	uvs.push_back(uv_up_left);
	uvs.push_back(uv_down_left);
	uvs.push_back(uv_up_right);

	uvs.push_back(uv_down_right);
	uvs.push_back(uv_up_right);
	uvs.push_back(uv_down_left);
	return uvs;
}
;


void Instance2D::Render(glm::mat4 parent_matrix,OpenGLContainer* open_gl) const{ //requires the proper shader to be bound!
	//float start_time = float(glfwGetTime());//[Performance Analysis]

	//cout << "Definition3D: render called" << endl;
	glBindVertexArray(VertexArrayID);

	// 1st buffer: Vertices
	glEnableVertexAttribArray(0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);

	//----------------------This step is expensive! Mostly the grabbing from the function----------------
	pair<float, float> offset_res;
	if (type != "Text") {
		offset_res = offset.Calc(pair<float, float>(-size_x / 2, -size_y / 2), pair<float, float>(size_x / 2, size_y / 2));
	}
	if (type == "Text") { //Text is arranged differently and not centered geometry wise by default(at least not in the horizontal)!
		offset_res = offset.Calc(pair<float, float>(0.0f, -size_y / 2), pair<float, float>(size_x, size_y / 2));
	}

	glm::mat4 offset_mat = glm::translate(mat4(), vec3(get<0>(offset_res) * transform.ScaleX(), get<1>(offset_res) * transform.ScaleYModded(open_gl), 0.0f));

	glm::mat4 ModelMatrix = parent_matrix * offset_mat * transform.MakeModelMatrix(open_gl) ; //The matrix on the right(definition model matrix) applies first, then the model spcific one 



	 //PV from gl_container since it doesn't change between objects
	 //MAYBE:Possibly roll this into the matrix we get as input?
	//glm::mat4 MVP = open_gl->PV * ModelMatrix;
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform

	//glUniformMatrix4fv(open_gl->MatrixID, 1, GL_FALSE, &MVP[0][0]);
	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform

	//glUniformMatrix4fv(open_gl->MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(open_gl->ModelMatrixID2D, 1, GL_FALSE, &ModelMatrix[0][0]);
	//glUniformMatrix4fv(open_gl->ViewMatrixID, 1, GL_FALSE, &open_gl->ViewMatrix[0][0]); //View matrix into opengl_container?

	glUniform1f(open_gl->Grey_interface,0.0f); //Get all of these from color subobject!
	glUniform1f(open_gl->C_R_interface, 1);
	glUniform1f(open_gl->C_G_interface, 1);
	glUniform1f(open_gl->C_B_interface, 1);
	glUniform1f(open_gl->Transparency_interface, 1);

	//-------------Set up texture stuff------------------
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, image);
	glUniform1i(open_gl->TextureID, 0);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindVertexArray(0);


}


void Instance2D::MakeSimpleRectangleMesh() {

	//Make a simple rectangle mesh!
	vector<glm::vec3> vertices; //for VBO
	vector<glm::vec2> uvs; //for VBO
	size_x = 2.0f; // size_x * transform.ScaleX(); <-- that is Transform2D buisness!
	size_y = 2.0f; // size_y * transform.ScaleY();

	glm::vec3 vertex_up_left = glm::vec3(-size_x / 2, size_y / 2, 0);
	glm::vec3 vertex_up_right = glm::vec3(size_x / 2, size_y / 2, 0);
	glm::vec3 vertex_down_right = glm::vec3(size_x / 2, -size_y / 2, 0);
	glm::vec3 vertex_down_left = glm::vec3(-size_x / 2, -size_y / 2, 0);
	vertices.push_back(vertex_up_left);
	vertices.push_back(vertex_down_left);
	vertices.push_back(vertex_up_right);
	vertices.push_back(vertex_down_right);
	vertices.push_back(vertex_up_right);
	vertices.push_back(vertex_down_left);

	glm::vec2 uv_up_left = glm::vec2(0, 0);
	glm::vec2 uv_up_right = glm::vec2(1, 0);
	glm::vec2 uv_down_right = glm::vec2(1, 1);
	glm::vec2 uv_down_left = glm::vec2(0, 1);
	uvs.push_back(uv_up_left);
	uvs.push_back(uv_down_left);
	uvs.push_back(uv_up_right);
	uvs.push_back(uv_down_right);
	uvs.push_back(uv_up_right);
	uvs.push_back(uv_down_left);

	MakeModel(vertices, uvs);

	vertex_count = 6;

	bool result_image = AttemptToLoadImage();
};