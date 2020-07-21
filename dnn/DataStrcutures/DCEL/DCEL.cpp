#include "DCEL.h"
#include "Point.h"
#include "Vector.h"
#include "../AVLTree/AVLTree.h"
#include <algorithm>
#include <cstdlib>
#include <iterator>
#include <vector>
#include <iostream>
#define M_PI 3.14159265358979323846
#define BUFFERSIZE 1000


Vertex::Vertex() : Point() {
	this->vertex_key = nullptr;
	this->incidentEdge = nullptr;
}

Vertex::Vertex(HEdge* _e) : Point() {
	this->vertex_key = nullptr;
	this->incidentEdge = _e;
}

Vertex::Vertex(Point* _p) : Point(_p) {
	this->vertex_key = nullptr;
	this->incidentEdge = nullptr;
}

Vertex::Vertex(Point* _p, HEdge* _e) : Point(_p) {
	this->vertex_key = nullptr;
	this->incidentEdge = _e;
}


Vertex::~Vertex() {

}

char* Vertex::getVertexKey() {
	return this->vertex_key;
}

void Vertex::setVertexKey(char* _k) {
	this->vertex_key = new char[strlen(_k) + 1];
	strcpy_s(this->vertex_key, strlen(_k) + 1, _k);
}

void Vertex::setIncidentEdge(HEdge *_e) {
	this->incidentEdge = _e;
}

HEdge* Vertex::getIncidentEdge() {
	return this->incidentEdge;
}

HEdge::HEdge() : Edge() {
	this->origin = nullptr;
	this->incidentFace = nullptr;
	this->next = nullptr;
	this->prev = nullptr;
	this->twin = nullptr;
}

HEdge::HEdge(Vertex *_v1, Vertex *_v2) : Edge(_v1, _v2) {
	this->origin = _v1;
	this->twin = new HEdge();
	this->twin->origin = _v2;
	this->twin->twin = this;
	this->incidentFace = nullptr;
	this->next = this->twin;
	this->prev = this->twin;
	this->twin->next = this;
	this->twin->prev = this;
	this->twin->s = _v2;
	this->twin->t = _v1;
}

HEdge::~HEdge() {
	if (!this->twin) {
		delete(this->twin);
	}
}

char* HEdge::getHedgeKey() {
	return this->hedge_key;
}

void HEdge::setHedgeKey(char* _k) {
	this->hedge_key = new char[strlen(_k) + 1];
	strcpy_s(this->hedge_key, strlen(_k) + 1, _k);

}

Vertex* HEdge::getOrigin() {
	return this->origin;
}

void HEdge::setOrigin(Vertex* _v) {
	this->origin = _v;
}


HEdge* HEdge::getNext() {
	return this->next;
}

void HEdge::setNext(HEdge* _e) {
	this->next = _e;
}


HEdge* HEdge::getPrev() {
	return this->prev;
}

void HEdge::setPrev(HEdge* _e) {
	this->prev = _e;
}


HEdge* HEdge::getTwin() {
	return this->twin;
}

void HEdge::setTwin(HEdge* _e) {
	this->twin = _e;
}


Face* HEdge::getIncidentFace() {
	return this->incidentFace;
}

void HEdge::setIncidentFace(Face *_f) {
	this->incidentFace = _f;
}

Face::Face() {
	this->outer = nullptr;
	this->inners = new std::vector<HEdge*>();
}

Face::~Face() {
	delete(this->inners);
}



char* Face::getFaceKey() {
	return this->face_key;
}

void Face::setFaceKey(char* _k) {
	this->face_key = new char[strlen(_k) + 1];
	strcpy_s(this->face_key, strlen(_k) + 1, _k);
}


bool Face::isOutMost() {
	return !outer;
}

void Face::setOuter(HEdge* _e) {
	this->outer = _e;
}

HEdge* Face::getOuter() {
	return this->outer;
}

std::vector<HEdge*>* Face::getInners() {
	return this->inners;
}

void Face::addInner(HEdge* _e) {
	this->inners->push_back(_e);
}

DCEL::DCEL() {
	this->faces = new std::vector<Face*>();
	this->hedges = new std::vector<HEdge*>();
	this->vertices = new std::vector<Vertex*>();
	lmost = nullptr;
	tmost = nullptr;
	bmost = nullptr;
	rmost = nullptr;
	Face *of = new Face();
	this->faces->push_back(of);
}

DCEL::DCEL(FILE* readFile) {
	char* buffer = new char[BUFFERSIZE];
	fgets(buffer, BUFFERSIZE, readFile);
	char* context = NULL;

	// Number of Faces HEdges and Vertex
	char* token = strtok_s(buffer, "\t, \n", &context);
	this->num_vertices = atoi(token);
	//std::cout << this->num_vertices << std::endl;

	token = strtok_s(NULL, "\t, \n", &context);
	this->num_faces = atoi(token);
	//std::cout << this->num_faces << std::endl;

	token = strtok_s(NULL, "\t, \n", &context);
	this->num_hedges = atoi(token);
	//std::cout << this->num_hedges << std::endl;

	this->vertices = new std::vector<Vertex*>(num_vertices);
	for (int i = 0; i < num_vertices; i++) {
		(*this->vertices)[i] = new Vertex();
	}

	this->faces = new std::vector<Face*>(num_faces);
	for (int i = 0; i < num_faces; i++) {
		(*this->faces)[i] = new Face();
	}
	this->hedges = new std::vector<HEdge*>(num_hedges);
	for (int i = 0; i < num_hedges; i++) {
		(*this->hedges)[i] = new HEdge();
	}

	//Set Keys of Faces, HEdges, and Vertices
	fgets(buffer, BUFFERSIZE, readFile);
	token = strtok_s(buffer, "\t, \n", &context);
	(*this->vertices)[0]->setVertexKey(token);

	for (int i = 1; i < this->num_vertices; i++) {
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->vertices)[i]->setVertexKey(token);
	}


	fgets(buffer, BUFFERSIZE, readFile);
	token = strtok_s(buffer, "\t, \n", &context);
	(*this->faces)[0]->setFaceKey(token);

	for (int i = 1; i < this->num_faces; i++) {
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->faces)[i]->setFaceKey(token);
	}

	fgets(buffer, BUFFERSIZE, readFile);
	token = strtok_s(buffer, "\t, \n", &context);
	(*this->hedges)[0]->setHedgeKey(token);

	for (int i = 1; i < this->num_hedges; i++) {
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->hedges)[i]->setHedgeKey(token);

	}

	// Set Vertex information
	for (int i = 0; i < num_vertices; i++) {
		fgets(buffer, BUFFERSIZE, readFile);
		token = strtok_s(buffer, "\t, \n", &context);
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->vertices)[i]->setx(atof(token));
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->vertices)[i]->sety(atof(token));
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->vertices)[i]->setIncidentEdge(this->searchHedge(token));

		if (this->lmost)  this->lmost = this->lmost->getx() > (*this->vertices)[i]->getx() ? (*this->vertices)[i] : this->lmost;
		else this->lmost = (*this->vertices)[i];
		if (this->rmost)  this->rmost = this->rmost->getx() < (*this->vertices)[i]->getx() ? (*this->vertices)[i] : this->rmost;
		else this->rmost = (*this->vertices)[i];
		if (this->bmost)  this->bmost = this->bmost->gety() > (*this->vertices)[i]->gety() ? (*this->vertices)[i] : this->bmost;
		else this->bmost = (*this->vertices)[i];
		if (this->tmost)  this->tmost = this->tmost->gety() < (*this->vertices)[i]->gety() ? (*this->vertices)[i] : this->tmost;
		else this->tmost = (*this->vertices)[i];
	}

	// Set Face information
	int num_inner_components = 0;
	for (int i = 0; i < num_faces; i++) {
		fgets(buffer, BUFFERSIZE, readFile);
		token = strtok_s(buffer, "\t, \n", &context);
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->faces)[i]->setOuter(this->searchHedge(token));
		token = strtok_s(NULL, "\t, \n", &context);
		num_inner_components = atoi(token);

		for (int j = 0; j < num_inner_components; j++) {
			token = strtok_s(NULL, "\t, \n", &context);
			(*this->faces)[i]->getInners()->push_back(this->searchHedge(token));
		}
	}

	// Set Half Edge information
	for (int i = 0; i < num_hedges; i++) {
		fgets(buffer, BUFFERSIZE, readFile);
		token = strtok_s(buffer, "\t, \n", &context);
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->hedges)[i]->setOrigin(this->searchVertex(token));
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->hedges)[i]->setTwin(this->searchHedge(token));
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->hedges)[i]->setIncidentFace(this->searchFace(token));
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->hedges)[i]->setNext(this->searchHedge(token));
		token = strtok_s(NULL, "\t, \n", &context);
		(*this->hedges)[i]->setPrev(this->searchHedge(token));
	}
}


DCEL::~DCEL() {
	delete(this->faces);
	delete(this->hedges);
	delete(this->vertices);
}


std::vector<Face*>* DCEL::getFaces() {
	return this->faces;
}

void DCEL::setFaces(std::vector<Face*>* _f) {
	this->faces = _f;
}

std::vector<HEdge*>* DCEL::getHedges() {
	return this->hedges;
}

void DCEL::setHedges(std::vector<HEdge*>* _e) {
	this->hedges = _e;
}


std::vector<Vertex*>* DCEL::getVertices() {
	return this->vertices;
}

void DCEL::setVertices(std::vector<Vertex*>* _v) {
	this->vertices = _v;
}

Vertex* DCEL::getLmost() { return this->lmost; }
Vertex* DCEL::getRmost() { return this->rmost; }
Vertex* DCEL::getTmost() { return this->tmost; }
Vertex* DCEL::getBmost() { return this->bmost; }

void DCEL::addEdge(Vertex* _v1, Vertex* _v2) {

	if (_v1->getx() > _v2->getx()) {
		std::swap(_v1, _v2);
	}

	double _x1 = _v1->getx();
	double _y1 = _v1->gety();
	double _x2 = _v2->getx();
	double _y2 = _v2->gety();
	char* _c1 = _v1->getVertexKey();
	char* _c2 = _v2->getVertexKey();
	std::string _str1(_c1);
	std::string _str2(_c2);
	_str1 = _str1.substr(1);
	_str1 = _str2.substr(2);
	std::string _str = "e" + _str1 + "_" + _str2;
	char* _c = &_str[0];// = new char[_str1.length() + _str1.length() + 2];
	HEdge* e = new HEdge(_v1, _v2);
	e->setHedgeKey(_c);
	HEdge* closest_e = nullptr;

	//edges
	//_v2를 공유하는 edge중에 더 기울기가 작은걸 찾음
	for (auto _e : this->getOutgoingHEdges(_v2)) {
		double min_angle = 2 * M_PI;
		double theta;

		Vertex* _v3 = _e->getTwin()->getOrigin();
		Vector* _v21 = new Vector(_v2, _v1);
		Vector* _v23 = new Vector(_v2, _v3);
		theta = acos(_v21->innerProdct(_v23) / (_v21->norm() * _v23->norm()));
		double z = _v21->outerProdct(_v23);
		if (z > 0) {
			theta += M_PI;
		}
		if (theta < min_angle) {
			min_angle = theta;
			closest_e = _e;
		}

	}
	if (closest_e == nullptr) {
		e->setNext(e->getTwin());
		e->getTwin()->setPrev(e);
		_v2->setIncidentEdge(e->getTwin());
	}
	else {
		e->getTwin()->setPrev(closest_e->getPrev());
		closest_e->getPrev()->setNext(e->getTwin());
		e->setNext(closest_e);
		closest_e->setPrev(e);
	}

	closest_e = nullptr;
	//_v1를 공유하는 edge중에 더 기울기가 작은걸 찾음
	for (auto _e : this->getOutgoingHEdges(_v1)) {
		double min_angle = 2 * M_PI;
		double theta;
		if (_e->getTwin()->getOrigin() != _v2) {
			Vertex* _v3 = _e->getTwin()->getOrigin();
			Vector* _v12 = new Vector(_v1, _v2);
			Vector* _v13 = new Vector(_v1, _v3);
			theta = acos(_v12->innerProdct(_v13) / (_v12->norm() * _v13->norm()));
			double z = _v12->outerProdct(_v13);
			if (z > 0) {//시계방향
				theta += M_PI;
			}
			if (theta < min_angle) {
				min_angle = theta;
				closest_e = _e;
				closest_e->setPrev(_e->getPrev());
			}
		}
	}
	if (closest_e == nullptr) {
		e->setPrev(e->getTwin());
		e->getTwin()->setNext(e);
	}
	else {
		e->setPrev(closest_e->getPrev());
		closest_e->getPrev()->setNext(e);
		e->getTwin()->setNext(closest_e);
		closest_e->setPrev(e->getTwin());
	}

	//faces
	HEdge* _e = e->getNext();
	while (_e != e && _e != e->getTwin()) {
		_e = _e->getNext();
	}
	//new face is made
	if (_e == e) {
		Face* f = e->getNext()->getIncidentFace();
		std::vector<HEdge*>* inners = f->getInners();
		HEdge* outer = f->getOuter();
		Face* f1 = new Face(); //e->getNext's new incedent face
		Face* f2 = new Face();//twin(e)->getNext's new incedent face
		e->setIncidentFace(f1);
		e->getTwin()->setIncidentFace(f2);
		f1->setOuter(e);
		f2->setOuter(e->getTwin());

		HEdge* temp = e->getNext();
		while (temp != e) {
			temp->setIncidentFace(f1);
			temp = temp->getNext();
		}
		temp = e->getTwin()->getNext();
		while (temp != e) {
			temp->setIncidentFace(f2);
			temp = temp->getNext();
		}
		//face inner setting
		for (auto temp_e : *inners) {
			SimplePolygon sp_f1 = SimplePolygon();
			HEdge* h_e = e;
			do {
				sp_f1.getEdges()->push_back(h_e);
				h_e = h_e->getNext();
			} while (h_e == e);
			if (sp_f1.inPolygon(temp_e->gets()) == 1) {///if temp_e in f1
				f1->getInners()->push_back(temp_e);
			}
			else {
				f2->getInners()->push_back(temp_e);
			}
		}

		int iter = 0;
		for (std::vector<Face*>::iterator i = this->getFaces()->begin(); i != this->getFaces()->end();) {
			Face* _f = *i;
			if (f == _f) {
				this->getFaces()->erase(i);
			}
		}

	}
	//new face is not made
	else if (_e == e->getTwin()) {
		e->setIncidentFace(e->getNext()->getIncidentFace());
		e->getTwin()->setIncidentFace(e->getIncidentFace());
	}
	this->getHedges()->push_back(e);
}

void DCEL::deleteEdge(HEdge* _e) {
	if (_e->getNext() == _e->getTwin()) {
		HEdge* _prev = _e->getPrev();
		_e->getPrev()->setNext(_e->getTwin()->getNext());
		_e->getTwin()->getNext()->setPrev(_e->getPrev());
		//delete e
		for (std::vector<HEdge*>::iterator i = this->getHedges()->begin(); i != this->getHedges()->end();) {
			Edge* temp = *i;
			if (temp == _e || temp == _e->getTwin()) {
				this->getHedges()->erase(i);
			}
		}
	}
	else if (_e->getPrev() == _e->getTwin()) {
		_e->getNext()->setPrev(_e->getTwin()->getPrev());
		_e->getTwin()->getPrev()->setNext(_e->getNext());
		for (std::vector<HEdge*>::iterator i = this->getHedges()->begin(); i != this->getHedges()->end();) {
			Edge* temp = *i;
			if (temp == _e || temp == _e->getTwin()) {
				this->getHedges()->erase(i);
			}
		}
	}
	else {
		HEdge* _temp = _e;
		do {
			_temp = _temp->getNext();
		} while (_temp == _e || _temp == _e->getTwin());

		//or maybe just compare their incident faces
		if (_temp == _e) {
			Face* f1 = _e->getIncidentFace();
			Face* f2 = _e->getTwin()->getIncidentFace();
			Face* f = new Face();
			f->setOuter(_e->getNext());
			HEdge* _temp2 = _e;
			do {
				_temp2->setIncidentFace(f);
				_temp2 = _temp2->getNext();
			} while (_temp2 == _e);

			_temp2 = _e->getTwin();
			do {
				_temp2->setIncidentFace(f);
				_temp2 = _temp2->getNext();
			} while (_temp2 == _e->getTwin());


			for (auto e : *(f1->getInners())) {
				f->addInner(_e);
			}
			for (auto e : *(f2->getInners())) {
				f->addInner(_e);
			}
			//face가 지워지는 경우
			_e->getNext()->setPrev(_e->getTwin()->getPrev());
			_e->getTwin()->getPrev()->setNext(_e->getNext());
			_e->getPrev()->setNext(_e->getTwin()->getNext());
			_e->getTwin()->getNext()->setPrev(_e->getPrev());
			//_e지움
			for (std::vector<HEdge*>::iterator i = this->getHedges()->begin(); i != this->getHedges()->end();) {
				Edge* temp = *i;
				if (temp == _e || temp == _e->getTwin()) {
					this->getHedges()->erase(i);
				}
			}
			//f1,f2지움
			for (std::vector<Face*>::iterator i = this->getFaces()->begin(); i != this->getFaces()->end();) {
				Face* _f = *i;
				if (_f == f1 || _f == f2) {
					this->getFaces()->erase(i);
				}
			}

		}
		else {
			//face가 지워지지 않는 경우
			_e->getNext()->setPrev(_e->getTwin()->getPrev());
			_e->getTwin()->getPrev()->setNext(_e->getNext());
			_e->getPrev()->setNext(_e->getTwin()->getNext());
			_e->getTwin()->getNext()->setPrev(_e->getPrev());
			//face에 혹시 
			_e->getIncidentFace()->setOuter(_e->getNext());
			//_e지움
			for (std::vector<HEdge*>::iterator i = this->getHedges()->begin(); i != this->getHedges()->end();) {
				Edge* temp = *i;
				if (temp == _e || temp == _e->getTwin()) {
					this->getHedges()->erase(i);
				}
			}
		}
	}
}

HEdge* DCEL::searchHedge(char* key) {
	for (int i = 0; i < this->getHedges()->size(); i++) {

		if (strcmp((*this->getHedges())[i]->getHedgeKey(), key) == 0) {
			return (*this->getHedges())[i];
		}
	}
	return nullptr;
}

Vertex* DCEL::searchVertex(char* key) {
	for (int i = 0; i < this->getVertices()->size(); i++) {
		if (strcmp((*this->getVertices())[i]->getVertexKey(), key) == 0) {
			return (*this->getVertices())[i];
		}
	}
	return nullptr;
}

Face* DCEL::searchFace(char* key) {
	for (int i = 0; i < this->getFaces()->size(); i++) {
		if (strcmp((*this->getFaces())[i]->getFaceKey(), key) == 0) {
			return (*this->getFaces())[i];
		}
	}
	return nullptr;
}

std::vector<HEdge*> DCEL::getOutgoingHEdges(Vertex* v) {
	std::vector<HEdge*> output;
	HEdge* e_original = v->getIncidentEdge();
	HEdge* e = e_original;
	do {
		output.push_back(e);
		e = e->getTwin()->getNext();
	} while (e != e_original);
	return output;
}

std::vector<HEdge*> DCEL::getIncomingHEdges(Vertex* v) {
	std::vector<HEdge*> output;
	HEdge* e_original = v->getIncidentEdge();
	HEdge* e = e_original;
	do {
		output.push_back(e->getTwin());
		e->getTwin()->getNext();
	} while (e != e_original);
	return output;
}

//The point p is inside polygon return 1
//The point p is on boundary return 0
//The point p is outside polygon return -1
int DCEL::inPolygon(std::vector<HEdge*> *hedges, Point p) {
	double min_x = 1e10;
	double min_x2 = 1e10;
	int idx = -1;
	double x = p.getx(), y = p.gety();
	for (int i = 0; i < hedges->size(); i++) {
		Edge* _e = (*(hedges))[i];
		Point origin = _e->gets(), dest = _e->gett();

		if (origin.gety() > dest.gety())	//y1 <= y2
			std::swap(origin, dest);
		double x1 = origin.getx(), y1 = origin.gety();
		double x2 = dest.getx(), y2 = dest.gety();

		//Point is on boundary line
		if (abs((y1 - y2) * (x - x1) - (y - y1) * (x1 - x2)) < 1e-6) {
			if ((std::min(y1, y2) <= y && y <= std::max(y1, y2)) && std::min(x1, x2) <= x && x <= std::max(x1, x2))
				return 0;
		}

		if (y1 - 1e-6 < y && y < y2 + 1e-6 && abs(y1 - y2) > 1e-6 && (y - y1) * (x2 - x1) > (x - x1)* (y2 - y1)) {//edge is right to point p
			double tmp = (y - y1) * (x1 - x2) / (y1 - y2) + x1;
			Point tmp_org = Point(x1, y1), tmp_dest = Point(x2, y2);
			if (abs(y - y2) < 1e-6) { tmp_org = Point(x2, y2); tmp_dest = Point(x1, y1); }
			//Normalize x2
			double normal_x2 = (tmp_dest - tmp_org).getx() / (tmp_dest - tmp_org).distance(tmp_dest - tmp_org) + tmp_org.getx(); //
			if (min_x > tmp || (abs(min_x - tmp) < 1e-6 && min_x2 > x2)) {
				min_x = tmp;
				min_x2 = x2;
				idx = i;
			}
		}
	}
	if (idx == -1) return -1;
	Point org = (*hedges)[idx]->gets();
	Point dest = (*hedges)[idx]->gett();
	if ((dest - org).gety() > 0)
		return -1;
	else return 1;
}

void DCEL::DCELtotext(FILE* readFile) {
	char* buffer = new char[BUFFERSIZE];
	sprintf_s(buffer, BUFFERSIZE, "%d\t%d\t%d\n", this->vertices->size(), this->faces->size(), this->hedges->size());
	fputs(buffer, readFile);

	int i = 0;
	for (i = 0; i < this->vertices->size() - 1; i++) {
		sprintf_s(buffer, BUFFERSIZE, "%s\t", (*this->vertices)[i]->getVertexKey());
		fputs(buffer, readFile);
	}
	if (i == this->vertices->size() - 1) {
		sprintf_s(buffer, BUFFERSIZE, "%s\n", (*this->vertices)[i]->getVertexKey());
		fputs(buffer, readFile);
	}

	for (i = 0; i < this->faces->size() - 1; i++) {
		sprintf_s(buffer, BUFFERSIZE, "%s\t", (*this->faces)[i]->getFaceKey());
		fputs(buffer, readFile);
	}
	if (i == this->faces->size() - 1) {
		sprintf_s(buffer, BUFFERSIZE, "%s\n", (*this->faces)[i]->getFaceKey());
		fputs(buffer, readFile);
	}

	for (i = 0; i < this->hedges->size() - 1; i++) {
		sprintf_s(buffer, BUFFERSIZE, "%s\t", (*this->hedges)[i]->getHedgeKey());
		fputs(buffer, readFile);
	}
	if (i == this->hedges->size() - 1) {
		sprintf_s(buffer, BUFFERSIZE, "%s\n", (*this->hedges)[i]->getHedgeKey());
		fputs(buffer, readFile);
	}
	for (i = 0; i < this->vertices->size(); i++)
	{
		sprintf_s(buffer, BUFFERSIZE, "%s\t%.3lf,%.3lf\t%s\n", (*this->vertices)[i]->getVertexKey(), (*this->vertices)[i]->getx(), (*this->vertices)[i]->gety(), (*this->vertices)[i]->getIncidentEdge()->getHedgeKey());
		fputs(buffer, readFile);
	}
	for (i = 0; i < this->faces->size(); i++)
	{
		sprintf_s(buffer, BUFFERSIZE, "%s\t", (*this->faces)[i]->getFaceKey());
		fputs(buffer, readFile);

		if ((*this->faces)[i]->getOuter() == nullptr) {
			sprintf_s(buffer, BUFFERSIZE, "NULL\t%d\t", (*this->faces)[i]->getInners()->size());
			fputs(buffer, readFile);
		}
		else {
			sprintf_s(buffer, BUFFERSIZE, "%s\t%d\t", (*this->faces)[i]->getOuter()->getHedgeKey(), (*this->faces)[i]->getInners()->size());
			fputs(buffer, readFile);
		}

		int j = 0;
		if ((*this->faces)[i]->getInners()->size() == 0) {
			fputs("NULL\n", readFile);
		}

		else {
			for (j = 0; j < (*this->faces)[i]->getInners()->size() - 1; j++) {
				sprintf_s(buffer, BUFFERSIZE, "%s\t", (*(*this->faces)[i]->getInners())[j]->getHedgeKey());
				fputs(buffer, readFile);
			}

			if (j == (*this->faces)[i]->getInners()->size() - 1) {
				sprintf_s(buffer, BUFFERSIZE, "%s\n", (*(*this->faces)[i]->getInners())[j]->getHedgeKey());
				fputs(buffer, readFile);
			}
		}
	}

	for (i = 0; i < this->hedges->size() - 1; i++) {
		sprintf_s(buffer, BUFFERSIZE, "%s\t%s\t%s\t%s\t%s\t%s\n", (*this->hedges)[i]->getHedgeKey(), (*this->hedges)[i]->getOrigin()->getVertexKey(), (*this->hedges)[i]->getTwin()->getHedgeKey(), (*this->hedges)[i]->getIncidentFace()->getFaceKey(), (*this->hedges)[i]->getNext()->getHedgeKey(), (*this->hedges)[i]->getPrev()->getHedgeKey());
		fputs(buffer, readFile);
	}
	if (i == this->hedges->size() - 1) {
		sprintf_s(buffer, BUFFERSIZE, "%s\t%s\t%s\t%s\t%s\t%s", (*this->hedges)[i]->getHedgeKey(), (*this->hedges)[i]->getOrigin()->getVertexKey(), (*this->hedges)[i]->getTwin()->getHedgeKey(), (*this->hedges)[i]->getIncidentFace()->getFaceKey(), (*this->hedges)[i]->getNext()->getHedgeKey(), (*this->hedges)[i]->getPrev()->getHedgeKey());
		fputs(buffer, readFile);
	}
}

void DCEL::printVertexTab() {
	std::cout << "\n" << "*********** Vertex Table ************" << "\n";
	std::cout << "vertex" << "\tCoordinates " << "\tIncident Edge " << "\n";

	for (int i = 0; i < this->vertices->size(); i++)
	{
		std::cout << std::setw(4) << (*this->vertices)[i]->getVertexKey() << std::setw(6) << "(" << std::setw(2) << (*this->vertices)[i]->getx() << ", " << std::setw(2) << (*this->vertices)[i]->gety() << ")" << std::setw(14) << (*this->vertices)[i]->getIncidentEdge()->getHedgeKey() << std::endl;
	}
}
void DCEL::printHedgeTab() {
	std::cout << "\n" << "****************** Half-edge Table ******************" << "\n";
	std::cout << "Half-edge " << " Origin " << "  Twin" << "  IncidentFace" << "  Next" << "    Prev" << "\n";
	for (int i = 0; i < this->hedges->size(); i++)
	{
		std::cout << std::setw(7) << (*this->hedges)[i]->getHedgeKey() << std::setw(8) << (*this->hedges)[i]->getOrigin()->getVertexKey() << std::setw(9) << (*this->hedges)[i]->getTwin()->getHedgeKey() << "\t" << (*this->hedges)[i]->getIncidentFace()->getFaceKey() << "\t" << (*this->hedges)[i]->getNext()->getHedgeKey() << "\t" << (*this->hedges)[i]->getPrev()->getHedgeKey() << std::endl;
	}
}

void DCEL::printFaceTab() {
	std::cout << "\n" << "************ Face Table *************" << "\n";
	std::cout << "Face " << " OuterComponent " << "InnerComponents" << "\n";
	for (int i = 0; i < this->faces->size(); i++)
	{
		std::cout << std::setw(4) << (*this->faces)[i]->getFaceKey() << "\t";

		if ((*this->faces)[i]->getOuter() == nullptr) {
			std::cout << " NULL \t\t";
		}
		else {
			std::cout << " " << (*this->faces)[i]->getOuter()->getHedgeKey() << "\t\t";
		}

		if (((*this->faces)[i]->getInners()->size() == 0)) {
			std::cout << "  NULL \t";

		}
		else {
			std::cout << "  ";
			for (int j = 0; j < (*this->faces)[i]->getInners()->size(); j++) {
				std::cout << (*(*this->faces)[i]->getInners())[j]->getHedgeKey() << "  ";
			}
		}
		std::cout << std::endl;
	}
}

DCEL* DCEL::mergeDCEL(DCEL* _d) {
	enum Event {
		START,
		END,
		CROSS
	};
	struct Eventtype;
	struct Edgetype {
		HEdge* he;
		double* curx;
		struct Eventtype *cd, *cu;
		bool operator<(struct Edgetype _e) {
			Point ts = *this->he->gets();
			Point tt = *this->he->gett();
			Point es = *_e.he->gets();
			Point et = *_e.he->gets();
			double ty, ey;
			if (ts.getx() == tt.getx())
				ty = ts.gety();
			else
				ty = (tt.gety() - ts.gety()) / (tt.getx() - ts.getx())*(*curx - ts.getx()) + ts.gety();
			if (es.getx() == et.getx())
				ey = es.gety();
			else
				ey = (et.gety() - es.gety()) / (et.getx() - es.getx())*(*curx - es.getx()) + es.gety();
			if (ty == ey) {
				if (ts.getx() == tt.getx())
					return true;
				else if (es.getx() == et.getx())
					return false;
				else {
					ty = (tt.gety() - ts.gety()) / (tt.getx() - ts.getx())*(*curx + 1 - ts.getx()) + ts.gety();
					ey = (et.gety() - es.gety()) / (et.getx() - es.getx())*(*curx + 1 - es.getx()) + es.gety();
					return ty < ey;
				}
			}
			else
				return ty < ey;
		}
		bool operator==(struct Edgetype _e) {
			return *he == *_e.he;
		}
	};
	struct Eventtype {
		struct Edgetype *e1, *e2;
		Point* eventPoint;
		Event ty;
		bool operator<(struct Eventtype _e) {
			if (*eventPoint == *_e.eventPoint) {
				if (ty == CROSS) {
					return true;
				}
				else if (_e.ty == CROSS) {
					return false;
				}
				else if ((ty == _e.ty) && (ty == START)) {
					if (e1->he->gets()->getx() == e1->he->gett()->getx())
						return false;
					else if (_e.e1->he->gets()->getx() == e1->he->gett()->getx())
						return true;
					else
						return (e1->he->gett()->gety() - eventPoint->gety()) / (e1->he->gett()->getx() - eventPoint->getx()) < (e1->he->gett()->gety() - eventPoint->gety()) / (e1->he->gett()->getx() - eventPoint->getx());
				}
				else if (ty == START)
					return false;
				else if (_e.ty == START) {
					return true;
				}
				else {
					if (e1->he->gets()->getx() == e1->he->gett()->getx())
						return false;
					else if (_e.e1->he->gets()->getx() == e1->he->gett()->getx())
						return true;
					else
						return (e1->he->gett()->gety() - eventPoint->gety()) / (e1->he->gett()->getx() - eventPoint->getx()) > (e1->he->gett()->gety() - eventPoint->gety()) / (e1->he->gett()->getx() - eventPoint->getx());
				}
			}
			else if (eventPoint->getx() == _e.eventPoint->getx())
				return eventPoint->gety() < eventPoint->gety();
			else
				return eventPoint->getx() < eventPoint->getx();
		}
	};
	DCEL* merged = new DCEL();
	AVLTree<struct Eventtype> events;
	AVLTree<struct Edgetype> curedges;
	double curx;
	for (int i = 0; i < this->hedges->size(); i++) {
		HEdge* he = (*this->hedges)[i];
		Edgetype* het = new Edgetype();
		AVLTreeNode<struct Eventtype> *henode = new AVLTreeNode<struct Eventtype>();
		henode->value.e1 = het;
		henode->value.e2 = nullptr;
		henode->value.ty = START;
		het->curx = &curx;
		if (he->gets()->getx() == he->gett()->getx())
			if (he->gets()->gety() > he->gett()->gety())
				het->he = he->getTwin();
			else
				het->he = he;
		else
			if (he->gets()->getx() < he->gett()->getx())
				het->he = he;
			else
				het->he = he->getTwin();
		henode->value.eventPoint = het->he->gets();
		events.insert(henode);
	}
	for (int i = 0; i < _d->hedges->size(); i++) {
		HEdge* he = (*_d->hedges)[i];
		Edgetype* het = new Edgetype();
		AVLTreeNode<struct Eventtype> *henode = new AVLTreeNode<struct Eventtype>();
		henode->value.e1 = het;
		henode->value.e2 = nullptr;
		henode->value.ty = START;
		het->curx = &curx;
		if (he->gets()->getx() == he->gett()->getx())
			if (he->gets()->gety() > he->gett()->gety())
				het->he = he->getTwin();
			else
				het->he = he;
		else
			if (he->gets()->getx() < he->gett()->getx())
				het->he = he;
			else
				het->he = he->getTwin();
		henode->value.eventPoint = het->he->gets();
		events.insert(henode);
	}
	Vertex* curv = nullptr;
	while (!events.isEmpty()) {
		AVLTreeNode<struct Eventtype>*curevent = events.pop();
		if (!curv || !(*curv == *curevent->value.eventPoint)) {
			curv = new Vertex(curevent->value.eventPoint);
			merged->getVertices()->push_back(curv);
			curx = curv->getx();
		}
		switch (curevent->value.ty) {
		case START:
			AVLTreeNode<struct Edgetype>*curedge = new AVLTreeNode<struct Edgetype>();
			HEdge*hedge = new HEdge(curevent->value.e1->he->gets(), curevent->value.e1->he->gett());
			curevent->value.e1->he = hedge;
			curedge->value = *curevent->value.e1;
			curedge->value.cd = nullptr;
			curedge->value.cu = nullptr;
			curedges.insert(curedge);
			AVLTreeNode<struct Edgetype>*upedge = curedges.getLeftNode(curedge->value);
			if (upedge) {
				Point* cp = hedge->crossing(upedge->value.he, false);
				if (cp) {
					if (upedge->value.cd) {
						events.pop(*upedge->value.cd);
						upedge->value.cd->e2->cu = nullptr;
						delete(upedge->value.cd);
					}
					AVLTreeNode<struct Eventtype>*ce = new AVLTreeNode<struct Eventtype>();
					ce->value.e2 = &curedge->value;
					ce->value.e1 = &upedge->value;
					ce->value.eventPoint = cp;
					ce->value.ty = CROSS;
					upedge->value.cd = &ce->value;
					events.insert(ce);
				}
			}
			AVLTreeNode<struct Edgetype>*downedge = curedges.getRightNode(curedge->value);
			if (downedge) {
				Point* cp = hedge->crossing(downedge->value.he, false);
				if (cp) {
					if (downedge->value.cu) {
						events.pop(*downedge->value.cu);
						downedge->value.cu->e1->cd = nullptr;
						delete(downedge->value.cu);
					}
					AVLTreeNode<struct Eventtype>*ce = new AVLTreeNode<struct Eventtype>();
					ce->value.e1 = &curedge->value;
					ce->value.e2 = &downedge->value;
					ce->value.eventPoint = cp;
					ce->value.ty = CROSS;
					events.insert(ce);
				}
			}
			break;
		case END:
			merged->addEdge(curevent->value.e1->he->gets(), curevent->value.e1->he->gett());
			AVLTreeNode<struct Edgetype>*upedge = curedges.getLeftNode(curedge->value);
			AVLTreeNode<struct Edgetype>*downedge = curedges.getRightNode(curedge->value);
			if (upedge&&downedge) {
				Point* cp = downedge->value.he->crossing(upedge->value.he, false);
				if (cp) {
					AVLTreeNode<struct Eventtype>*ce = new AVLTreeNode<struct Eventtype>();
					ce->value.e1 = &upedge->value;
					ce->value.e2 = &downedge->value;
					ce->value.eventPoint = cp;
					ce->value.ty = CROSS;
					events.insert(ce);
				}
			}
			delete(curevent->value.e1);
			break;
		case CROSS:
			HEdge *he1, *he2;
			he1 = new HEdge(curevent->value.eventPoint, curevent->value.e1->he->gett());
			he2 = new HEdge(curevent->value.eventPoint, curevent->value.e2->he->gett());
			curevent->value.e1->he->sett(curevent->value.eventPoint);
			curevent->value.e2->he->sett(curevent->value.eventPoint);
			AVLTreeNode<struct Eventtype> *e1, *e2, *s1, *s2;
			e1 = new AVLTreeNode<struct Eventtype>();
			e2 = new AVLTreeNode<struct Eventtype>();
			s1 = new AVLTreeNode<struct Eventtype>();
			s2 = new AVLTreeNode<struct Eventtype>();
			e1->value.eventPoint = curevent->value.eventPoint;
			e2->value.eventPoint = curevent->value.eventPoint;
			s1->value.eventPoint = curevent->value.eventPoint;
			s2->value.eventPoint = curevent->value.eventPoint;
			e1->value.ty = END;
			e2->value.ty = END;
			s1->value.ty = START;
			s2->value.ty = START;
			s1->value.e1 = curevent->value.e1;
			s2->value.e1 = curevent->value.e2;
			e1->value.e1 = new struct Edgetype();
			e1->value.e2 = nullptr;
			e1->value.e1->curx = &curx;
			e1->value.e1->he = he1;
			e1->value.e1->cd = nullptr;
			e1->value.e1->cu = nullptr;
			e2->value.e1 = new struct Edgetype();
			e2->value.e2 = nullptr;
			e2->value.e2->curx = &curx;
			e2->value.e2->he = he2;
			e2->value.e2->cd = nullptr;
			e2->value.e2->cu = nullptr;
			events.insert(e1);
			events.insert(e2);
			events.insert(s1);
			events.insert(s2);
			break;
		default:
			break;
		}
		delete(curevent);
	}
	return merged;
}
