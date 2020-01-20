#ifndef _C_IRR_ODE_CAPSULE
  #define _C_IRR_ODE_CAPSULE

  #include <geom/CIrrOdeGeom.h>

namespace irr {
namespace ode {

const int IRR_ODE_GEOM_CAPSULE_ID=MAKE_IRR_ID('i','o','g','u');
const wchar_t IRR_ODE_GEOM_CAPSULE_NAME[0xFF]=L"CIrrOdeGeomCapsule";

class CIrrOdeGeomCapsule : public CIrrOdeGeom {
  protected:
    f32 m_fRadius,m_fLength;

  public:
    CIrrOdeGeomCapsule(ISceneNode *parent,ISceneManager *mgr,s32 id = -1,
                   const vector3df &position=core::vector3df(0,0,0),
		               const vector3df &rotation = core::vector3df(0,0,0),
		               const vector3df &scale = core::vector3df(1.0f, 1.0f, 1.0f));

    virtual ~CIrrOdeGeomCapsule();

    virtual void render();
    virtual void OnRegisterSceneNode();
    virtual void initPhysics();

    virtual ESCENE_NODE_TYPE getType() const;
    virtual s32 getID() const;
    virtual const wchar_t *getTypeName();

    virtual void setMassTotal(f32 fMass);

    void setRadius(f32 fRadius);
    void setLength(f32 fLength);

  	virtual ISceneNode *clone(ISceneNode* newParent=0, ISceneManager* newManager=0);
  	virtual void copyParams(CIrrOdeSceneNode *pDest, bool bRecurse=true);

    virtual void serializeAttributes(IAttributes* out, SAttributeReadWriteOptions* options) const;
    virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options);
};

} //namespace ode
} //namespace irr

#endif
