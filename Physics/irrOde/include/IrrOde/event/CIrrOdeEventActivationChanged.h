#ifndef _IRR_ODE_EVENT_ACTIVATION_CHANGED
  #define _IRR_ODE_EVENT_ACTIVATION_CHANGED

  #include <irrlicht.h>
  #include <event/IIrrOdeEvent.h>
  #include <CIrrOdeBody.h>
  #include <geom/CIrrOdeGeom.h>

namespace irr {
namespace ode {

/**
 * @class CIrrOdeEventActivationChanged
 * This event is posted whenever the activation of a body changes
 */
class CIrrOdeEventActivationChanged : public IIrrOdeEvent {
  protected:
    CIrrOdeBody *m_pBody; /*!< the body that changed */
    bool m_bActive;       /*!< the new state of the body */

  public:
    CIrrOdeEventActivationChanged(CIrrOdeBody *pBody, bool bActive, s32 ID=-1);
    CIrrOdeEventActivationChanged(CSerializer *pData, ISceneManager *pSmgr, s32 ID=-1);
    CIrrOdeEventActivationChanged(IIrrOdeEvent *pIn);

    virtual s32 getID();
    virtual u16 getType();

    CIrrOdeBody *getBody();
    bool isActive();

    virtual CSerializer *serialize();

    virtual const c8 *toString();
};

} //namespace ode
} //namespace irr

#endif

