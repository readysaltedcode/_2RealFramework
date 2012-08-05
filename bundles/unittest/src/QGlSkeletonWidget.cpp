#include "QGlSkeletonWidget.h"

QGlSkeletonWidget::QGlSkeletonWidget( QWidget *parent) : m_SceneLimit(_2Real::Point(-1,-1,-1), _2Real::Point(1,1,1)), m_bIsGlobal(false), QGLViewer(parent) 
{
}

QGlSkeletonWidget::~QGlSkeletonWidget()
{
}

void QGlSkeletonWidget::init()
{
	setSceneBoundingBox(qglviewer::Vec(m_SceneLimit.getMin().x(), m_SceneLimit.getMin().y(), m_SceneLimit.getMin().z()), 
			qglviewer::Vec(m_SceneLimit.getMax().x(), m_SceneLimit.getMax().y(), m_SceneLimit.getMax().z()));
	camera()->showEntireScene();
	setAxisIsDrawn(true);
}

void QGlSkeletonWidget::draw()
{
	makeCurrent();

	glClear( GL_COLOR_BUFFER_BIT );
	glColor4f(1,0,0,1);
	glDisable(GL_LIGHTING);
	glPointSize(5);
	
	for(int i=0; i<m_Skeletons.size(); i++)
	{
		std::vector<_2Real::RigidBody> rigidBodies = m_Skeletons[i].getRigidBodies();
		for(int j=0; j<rigidBodies.size(); j++)
		{
			std::vector<_2Real::Point> joints = rigidBodies[j].getPoints();
			for(int k=0; k<joints.size(); k++)
			{
				double x = -1.0 + 2.0 * (double(joints[k].x())); 
				double y = -1.0 + 2.0 * (1.0 - double(joints[k].y())); 
				double z = double(joints[k].z());

				glBegin(GL_POINTS);
					glVertex3f(x,y,z);
				glEnd();
			}
		}
	}
	glFlush();
}

void QGlSkeletonWidget::updateSkeleton( const _2Real::Skeleton& skeleton  )
{
	m_Skeletons.clear();
	m_Skeletons.push_back(skeleton);
	setSceneLimits(m_Skeletons.front().getLimit());
	update();
}

void QGlSkeletonWidget::updateSkeletons( const std::vector<_2Real::Skeleton>& skeletons  )
{
	m_Skeletons.clear();
	m_Skeletons = skeletons;
	if(m_Skeletons.size()>0)
	{
		setSceneLimits(m_Skeletons.front().getLimit());
	}
	update();
}


void QGlSkeletonWidget::setSceneLimits(_2Real::BoundingBox sceneLimit)
{
	makeCurrent();
	if(sceneLimit != m_SceneLimit)	//only set scene limit when it change, otherwise user wants to zoom in and out normally
	{
		m_SceneLimit = sceneLimit;
		setSceneBoundingBox(qglviewer::Vec(m_SceneLimit.getMin().x(), m_SceneLimit.getMin().y(), m_SceneLimit.getMin().z()), 
			qglviewer::Vec(m_SceneLimit.getMax().x(), m_SceneLimit.getMax().y(), m_SceneLimit.getMax().z()));
		camera()->showEntireScene();
	}
}
