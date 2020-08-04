﻿#include "openvroverlaycontroller.h"
#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QQuickView>
#include <QApplication>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsEllipseItem>
#include <QOpenGLExtraFunctions>
#include <QCursor>
#include <QProcess>
#include <QMessageBox>
#include <exception>
#include <iostream>
#include <cmath>
#include <openvr.h>
#include <locale>
#include <codecvt>
#include <buttonHandlers.h>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <comdef.h>
#include <chrono>
#include <Logging.h>

std::map<int, const char*> OverlayController::_openVRButtonNames = {
	{ 0, "System" },
	{ 1, "ApplicationMenu" },
	{ 2, "Grip" },
	{ 3, "DPad_Left" },
	{ 4, "DPad_Up" },
	{ 5, "DPad_Right" },
	{ 6, "DPad_Down" },
	{ 7, "Button_A" },
	{ 31, "ProximitySensor" },
	{ 32, "Axis0" },
	{ 33, "Axis1" },
	{ 34, "Axis2" },
	{ 35, "Axis3" },
	{ 36, "Axis4" },
};


std::vector<std::pair<std::string, WORD>> OverlayController::_keyboardVirtualCodes = {
	{ "<None>", 0x00 },
	/*{ "Left Mouse Button", VK_LBUTTON },
	{ "Right Mouse Button", VK_RBUTTON },
	{ "Middle Mouse Button", VK_MBUTTON },*/
	{ "Backspace", VK_BACK },
	{ "Tab", VK_TAB },
	{ "Clear", VK_CLEAR },
	{ "Enter", VK_RETURN },
	{ "Pause", VK_PAUSE },
	{ "Caps Lock", VK_CAPITAL },
	{ "ESC", VK_ESCAPE },
	{ "Spacebar", VK_SPACE },
	{ "Page Up", VK_PRIOR },
	{ "Page Down", VK_NEXT },
	{ "End", VK_END },
	{ "Home", VK_HOME },
	{ "Left Arrow", VK_LEFT },
	{ "Up Arrow", VK_UP },
	{ "Right Arrow", VK_RIGHT },
	{ "Down Arrow", VK_DOWN },
	{ "Select", VK_SELECT },
	{ "Print", VK_PRINT },
	{ "Insert", VK_INSERT },
	{ "Delete", VK_DELETE },
	{ "Help", VK_HELP },
	{ "0", 0x30 },
	{ "1", 0x31 },
	{ "2", 0x32 },
	{ "3", 0x33 },
	{ "4", 0x34 },
	{ "5", 0x35 },
	{ "6", 0x36 },
	{ "7", 0x37 },
	{ "8", 0x38 },
	{ "9", 0x39 },
	{ "A", 0x41 },
	{ "B", 0x42 },
	{ "C", 0x43 },
	{ "D", 0x44 },
	{ "E", 0x45 },
	{ "F", 0x46 },
	{ "G", 0x47 },
	{ "H", 0x48 },
	{ "I", 0x49 },
	{ "J", 0x4A },
	{ "K", 0x4B },
	{ "L", 0x4C },
	{ "M", 0x4D },
	{ "N", 0x4E },
	{ "O", 0x4F },
	{ "P", 0x50 },
	{ "Q", 0x51 },
	{ "R", 0x52 },
	{ "S", 0x53 },
	{ "T", 0x54 },
	{ "U", 0x55 },
	{ "V", 0x56 },
	{ "W", 0x57 },
	{ "X", 0x58 },
	{ "Y", 0x59 },
	{ "Z", 0x5A },
	{ "Left Windows Key", VK_LWIN },
	{ "Right Windows Key", VK_RWIN },
	{ "Numeric Keypad 0", VK_NUMPAD0 },
	{ "Numeric Keypad 1", VK_NUMPAD1 },
	{ "Numeric Keypad 2", VK_NUMPAD2 },
	{ "Numeric Keypad 3", VK_NUMPAD3 },
	{ "Numeric Keypad 4", VK_NUMPAD4 },
	{ "Numeric Keypad 5", VK_NUMPAD5 },
	{ "Numeric Keypad 6", VK_NUMPAD6 },
	{ "Numeric Keypad 7", VK_NUMPAD7 },
	{ "Numeric Keypad 8", VK_NUMPAD8 },
	{ "Numeric Keypad 9", VK_NUMPAD9 },
	{ "Multiply", VK_MULTIPLY },
	{ "Add", VK_ADD },
	{ "Separator", VK_SEPARATOR },
	{ "Subtract", VK_SUBTRACT },
	{ "Decimal", VK_DECIMAL },
	{ "Divide", VK_DIVIDE },
	{ "F1", VK_F1 },
	{ "F2", VK_F2 },
	{ "F3", VK_F3 },
	{ "F4", VK_F4 },
	{ "F5", VK_F5 },
	{ "F6", VK_F6 },
	{ "F7", VK_F7 },
	{ "F8", VK_F8 },
	{ "F9", VK_F9 },
	{ "F10", VK_F10 },
	{ "F11", VK_F11 },
	{ "F12", VK_F12 },
	{ "F13", VK_F13 },
	{ "F14", VK_F14 },
	{ "F15", VK_F15 },
	{ "F16", VK_F16 },
	{ "F17", VK_F17 },
	{ "F18", VK_F18 },
	{ "F19", VK_F19 },
	{ "F20", VK_F20 },
	{ "F21", VK_F21 },
	{ "F22", VK_F22 },
	{ "F23", VK_F23 },
	{ "F24", VK_F24 },
	{ "Num Lock", VK_NUMLOCK },
	{ "Scroll Lock", VK_SCROLL },
	{ "Left Shift", VK_LSHIFT },
	{ "Right Shift", VK_RSHIFT },
	{ "Left Ctrl", VK_LCONTROL },
	{ "Right Ctrl", VK_RCONTROL },
	{ "Left Alt", VK_LMENU },
	{ "Right Alt", VK_RMENU },
	{ "Volume Mute", VK_VOLUME_MUTE },
	{ "Volume Down", VK_VOLUME_DOWN },
	{ "Volume Up", VK_VOLUME_UP },
	{ "Media Next Track", VK_MEDIA_NEXT_TRACK },
	{ "Media Previous Track", VK_MEDIA_PREV_TRACK },
	{ "Media Stop", VK_MEDIA_STOP },
	{ "Media Play/Pause", VK_MEDIA_PLAY_PAUSE },
	{ "+", VK_OEM_PLUS },
	{ "-", VK_OEM_MINUS },
	{ ",", VK_OEM_COMMA },
	{ ".", VK_OEM_PERIOD },
};


/* These key codes have different meaning depending on the active keyboard layout, or may not exist at all */
std::vector<WORD> OverlayController::_keyboardVaryingVirtualCodes = {
	VK_OEM_1,
	VK_OEM_2,
	VK_OEM_3,
	VK_OEM_4,
	VK_OEM_5,
	VK_OEM_6,
	VK_OEM_7,
	VK_OEM_8,
	VK_OEM_102,
};


std::unique_ptr<OverlayController> OverlayController::singleton;

QSettings* OverlayController::_appSettings = nullptr;

OverlayController::~OverlayController() {
	Shutdown();
}

void OverlayController::Init(QQmlEngine* qmlEngine) {

	LOG(INFO) << u8"今、OpenVRとのコネクトを保留中…";
	// Loading the OpenVR Runtime
	auto initError = vr::VRInitError_None;
	vr::VR_Init(&initError, vr::VRApplication_Overlay);
	if (initError != vr::VRInitError_None) {
		if (initError == vr::VRInitError_Init_HmdNotFound || initError == vr::VRInitError_Init_HmdNotFoundPresenceFailed) {
			QMessageBox::critical(nullptr, "KinectToVR EX Overlay", "Could not find HMD!");
		}
		throw std::runtime_error(std::string("Failed to initialize OpenVR: ") + std::string(vr::VR_GetVRInitErrorAsEnglishDescription(initError)));
	}

	//m_runtimePathUrl = QUrl::fromLocalFile(vr::VR_RuntimePath());

	/* Handle keyboard layout dependent virtual key codes */
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	for (auto vc : _keyboardVaryingVirtualCodes) {
		auto raw = MapVirtualKey(vc, MAPVK_VK_TO_CHAR);
		if (raw != 0) {
			auto lc = (wchar_t)(raw & 0xFFFF);
			std::string name = converter.to_bytes(std::wstring(1, lc));
			_keyboardVirtualCodes.push_back({ name, vc });
		}
	}

	LOG(INFO) << u8"サウンドファイルのセットアップ…";
	QString activationSoundFile = m_runtimePathUrl.toLocalFile().append("/content/panorama/sounds/activation.wav");
	QFileInfo activationSoundFileInfo(activationSoundFile);

	QString focusChangedSoundFile = m_runtimePathUrl.toLocalFile().append("/content/panorama/sounds/focus_change.wav");
	QFileInfo focusChangedSoundFileInfo(focusChangedSoundFile);

	QSurfaceFormat format;
	// Qt's QOpenGLPaintDevice is not compatible with OpenGL versions >= 3.0
	// NVIDIA does not care, but unfortunately AMD does
	// Are subtle changes to the semantics of OpenGL functions actually covered by the compatibility profile,
	// and this is an AMD bug?
	format.setVersion(2, 1);
	//format.setProfile( QSurfaceFormat::CompatibilityProfile );
	format.setDepthBufferSize(16);
	format.setStencilBufferSize(8);
	format.setSamples(16);

	LOG(INFO) << u8"OpenGLのコンテキストのイニシャライズ…";
	m_pOpenGLContext.reset(new QOpenGLContext());
	m_pOpenGLContext->setFormat(format);
	if (!m_pOpenGLContext->create()) {
		throw std::runtime_error("Could not create OpenGL context");
	}

	// create an offscreen surface to attach the context and FBO to
	m_pOffscreenSurface.reset(new QOffscreenSurface());
	m_pOffscreenSurface->setFormat(m_pOpenGLContext->format());
	m_pOffscreenSurface->create();
	m_pOpenGLContext->makeCurrent(m_pOffscreenSurface.get());

	if (!vr::VROverlay()) {
		QMessageBox::critical(nullptr, "KinectToVR EX Overlay", "Is OpenVR running?");
		throw std::runtime_error(std::string("No Overlay interface"));
	}

	LOG(INFO) << u8"QMLのコンテキストをオーバーレイのコントローラーのために登録…";
	// Set qml context
	qmlEngine->rootContext()->setContextProperty("applicationVersion", getVersionString());
	qmlEngine->rootContext()->setContextProperty("vrRuntimePath", getVRRuntimePathUrl());

	LOG(INFO) << u8"シングルトンをオーバーレイのコントローラーのために登録…";
	// Register qml singletons
	qmlRegisterSingletonType<OverlayController>(applicationKey, 1, 0, "OverlayController", [](QQmlEngine*, QJSEngine*) {
		QObject* obj = getInstance();
		QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
		return obj;
		});
}

void OverlayController::Shutdown() {
	LOG(INFO) << u8"シャットダウンを呼ばれる。";
	if (m_pPumpEventsTimer) {
		disconnect(m_pPumpEventsTimer.get(), SIGNAL(timeout()), this, SLOT(OnTimeoutPumpEvents()));
		m_pPumpEventsTimer->stop();
		m_pPumpEventsTimer.reset();
	}
	if (m_pRenderTimer) {
		disconnect(m_pRenderControl.get(), SIGNAL(renderRequested()), this, SLOT(OnRenderRequest()));
		disconnect(m_pRenderControl.get(), SIGNAL(sceneChanged()), this, SLOT(OnRenderRequest()));
		disconnect(m_pRenderTimer.get(), SIGNAL(timeout()), this, SLOT(renderOverlay()));
		m_pRenderTimer->stop();
		m_pRenderTimer.reset();
	}
	m_pWindow.reset();
	m_pRenderControl.reset();
	m_pFbo.reset();
	m_pOpenGLContext.reset();
	m_pOffscreenSurface.reset();
}


void OverlayController::SetWidget(QQuickItem* quickItem, const std::string& name, const std::string& key) {
	LOG(INFO) << u8"オーバーレイのセットアップを始めた…";
	if (!desktopMode) {
		vr::VROverlayError overlayError = vr::VROverlay()->CreateDashboardOverlay(key.c_str(), name.c_str(), &m_ulOverlayHandle, &m_ulOverlayThumbnailHandle);
		if (overlayError != vr::VROverlayError_None) {
			if (overlayError == vr::VROverlayError_KeyInUse) {
				throw new std::exception("Overlay key in use");
			}
		}
		LOG(INFO) << u8"オーバーレイのプロパティのセットアップを始めた…";
		vr::VROverlay()->SetOverlayWidthInMeters(m_ulOverlayHandle, 2.5f);
		vr::VROverlay()->SetOverlayInputMethod(m_ulOverlayHandle, vr::VROverlayInputMethod_Mouse);
		vr::VROverlay()->SetOverlayFlag(m_ulOverlayHandle, vr::VROverlayFlags::VROverlayFlags_SendVRSmoothScrollEvents, true);
		std::string thumbIconPath = QApplication::applicationDirPath().toStdString() + "\\resources\\thumbicon.png";
		if (QFile::exists(QString::fromStdString(thumbIconPath))) {
			vr::VROverlay()->SetOverlayFromFile(m_ulOverlayThumbnailHandle, thumbIconPath.c_str());
		}

		LOG(INFO) << u8"QTimerのセットアップ…";
		// Too many render calls in too short time overwhelm Qt and an assertion gets thrown.
		// Therefore we use an timer to delay render calls
		m_pRenderTimer.reset(new QTimer());
		m_pRenderTimer->setSingleShot(true);
		m_pRenderTimer->setInterval(5);
		connect(m_pRenderTimer.get(), SIGNAL(timeout()), this, SLOT(renderOverlay()));

		QOpenGLFramebufferObjectFormat fboFormat;
		fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
		fboFormat.setTextureTarget(GL_TEXTURE_2D);
		m_pFbo.reset(new QOpenGLFramebufferObject(quickItem->width(), quickItem->height(), fboFormat));

		m_pRenderControl.reset(new QQuickRenderControl());
		m_pWindow.reset(new QQuickWindow(m_pRenderControl.get()));
		m_pWindow->setRenderTarget(m_pFbo.get());
		quickItem->setParentItem(m_pWindow->contentItem());
		m_pWindow->setGeometry(0, 0, quickItem->width(), quickItem->height());
		m_pRenderControl->initialize(m_pOpenGLContext.get());

		vr::HmdVector2_t vecWindowSize = {
			(float)quickItem->width(),
			(float)quickItem->height()
		};
		vr::VROverlay()->SetOverlayMouseScale(m_ulOverlayHandle, &vecWindowSize);

		connect(m_pRenderControl.get(), SIGNAL(renderRequested()), this, SLOT(OnRenderRequest()));
		connect(m_pRenderControl.get(), SIGNAL(sceneChanged()), this, SLOT(OnRenderRequest()));

		LOG(INFO) << u8"OverlayのQQuickWindowのフラグのセットアップを始めた…";
		m_pWindow->setFlags(Qt::FramelessWindowHint);
		m_pWindow->setColor(QColorConstants::Transparent);
		LOG(INFO) << u8"Overlayのウィジェットのセットアップの設定が正常に完了";
	}

	m_pPumpEventsTimer.reset(new QTimer());
	connect(m_pPumpEventsTimer.get(), SIGNAL(timeout()), this, SLOT(OnTimeoutPumpEvents()));
	m_pPumpEventsTimer->setInterval(20);
	m_pPumpEventsTimer->start();
}


void OverlayController::OnRenderRequest() {
	if (m_pRenderTimer && !m_pRenderTimer->isActive()) {
		m_pRenderTimer->start();
	}
}

void OverlayController::renderOverlay() {
	if (!desktopMode) {
		// get overlay visibility
		overlayVisible = vr::VROverlay()->IsOverlayVisible(m_ulOverlayHandle);

		// skip rendering if the overlay isn't visible
		if (!vr::VROverlay() || !vr::VROverlay()->IsOverlayVisible(m_ulOverlayHandle) && !vr::VROverlay()->IsOverlayVisible(m_ulOverlayThumbnailHandle))
			return;

		m_pRenderControl->polishItems();
		m_pRenderControl->sync();
		m_pRenderControl->render();

		GLuint unTexture = m_pFbo->texture();
		if (unTexture != 0) {
#if defined _WIN64 || defined _LP64
			// To avoid any compiler warning because of cast to a larger pointer type (warning C4312 on VC)
			vr::Texture_t texture = { (void*)((uint64_t)unTexture), vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#else
			vr::Texture_t texture = { (void*)unTexture, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#endif
			vr::VROverlay()->SetOverlayTexture(m_ulOverlayHandle, &texture);
		}
		m_pOpenGLContext->functions()->glFlush(); // We need to flush otherwise the texture may be empty.*/
	}
}


bool OverlayController::getOverlayTexture(vr::Texture_t& texture) {
	GLuint unTexture = m_pFbo->texture();
	if (unTexture != 0) {
#if defined _WIN64 || defined _LP64
		// To avoid any compiler warning because of cast to a larger pointer type (warning C4312 on VC)
		texture = { (void*)((uint64_t)unTexture), vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#else
		texture = { (void*)unTexture, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
#endif
		return true;
	}
	else {
		return false;
	}
}


void OverlayController::OnTimeoutPumpEvents() {
	if (!vr::VRSystem())
		return;

	vr::VREvent_t vrEvent;
	while (vr::VROverlay()->PollNextOverlayEvent(m_ulOverlayHandle, &vrEvent, sizeof(vrEvent))) {
		switch (vrEvent.eventType) {
		case vr::VREvent_MouseMove: {
			QPoint ptNewMouse(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
            if (ptNewMouse != m_ptLastMouse) {
				QMouseEvent mouseEvent(QEvent::MouseMove,
					QPoint(ptNewMouse.x(), 2295 - ptNewMouse.y()),
					QPoint(m_pWindow->mapToGlobal(ptNewMouse).x(),
						2295 - m_pWindow->mapToGlobal(ptNewMouse).y()),
					Qt::NoButton, m_lastMouseButtons, 0);
				m_ptLastMouse = ptNewMouse;
				QCoreApplication::sendEvent(m_pWindow.get(), &mouseEvent);
				OnRenderRequest();
            }
		}
								  break;

		case vr::VREvent_MouseButtonDown: {
			QPoint ptNewMouse(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
			Qt::MouseButton button = vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;
			m_lastMouseButtons |= button;
			QMouseEvent mouseEvent(QEvent::MouseButtonPress,
				QPoint(ptNewMouse.x(), 2295 - ptNewMouse.y()),
				QPoint(m_pWindow->mapToGlobal(ptNewMouse).x(),
					2295 - m_pWindow->mapToGlobal(ptNewMouse).y()),
				button, m_lastMouseButtons, 0);
			QApplication::instance()->sendEvent(m_pWindow.get(), &mouseEvent);
		}
										break;

		case vr::VREvent_MouseButtonUp: {
			QPoint ptNewMouse(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
			Qt::MouseButton button = vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;
			m_lastMouseButtons &= ~button;
			QMouseEvent mouseEvent(QEvent::MouseButtonRelease,
				QPoint(ptNewMouse.x(), 2295 - ptNewMouse.y()),
				QPoint(m_pWindow->mapToGlobal(ptNewMouse).x(),
					2295 - m_pWindow->mapToGlobal(ptNewMouse).y()),
				button, m_lastMouseButtons, 0);
			QApplication::instance()->sendEvent(m_pWindow.get(), &mouseEvent);
		}
									  break;

		case vr::VREvent_OverlayShown: {
			m_pWindow->update();
		}
									 break;

		case vr::VREvent_Quit: {
			vr::VRSystem()->AcknowledgeQuit_Exiting(); // Let us buy some time just in case
			Shutdown();
			QApplication::exit();
			return;
		}
							 break;

		case vr::VREvent_DashboardActivated: {
			dashboardVisible = true;
		}
										   break;

		case vr::VREvent_DashboardDeactivated: {
			dashboardVisible = false;
		}
											 break;

		case vr::VREvent_KeyboardDone: {
			char keyboardBuffer[1024];
			vr::VROverlay()->GetKeyboardText(keyboardBuffer, 1024);
			emit keyBoardInputSignal(QString(keyboardBuffer), vrEvent.data.keyboard.uUserValue);
		}
									 break;

		}
	}

	vr::TrackedDevicePose_t devicePoses[vr::k_unMaxTrackedDeviceCount];
	vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseStanding, 0.0f, devicePoses, vr::k_unMaxTrackedDeviceCount);

	if (m_ulOverlayThumbnailHandle != vr::k_ulOverlayHandleInvalid) {
		while (vr::VROverlay()->PollNextOverlayEvent(m_ulOverlayThumbnailHandle, &vrEvent, sizeof(vrEvent))) {
			switch (vrEvent.eventType) {
			case vr::VREvent_OverlayShown: {
				m_pWindow->update();
			}
										 break;
			}
		}
	}
}

QString OverlayController::getVersionString() {
	return QString(applicationVersionString);
}


QUrl OverlayController::getVRRuntimePathUrl() {
	return m_runtimePathUrl;
}


bool OverlayController::soundDisabled() {
	return noSound;
}

unsigned OverlayController::getNewUniqueNumber() {
	return m_uniqueNumber.fetch_add(1);
}


const vr::VROverlayHandle_t& OverlayController::overlayHandle() {
	return m_ulOverlayHandle;
}


const vr::VROverlayHandle_t& OverlayController::overlayThumbnailHandle() {
	return m_ulOverlayThumbnailHandle;
}


void OverlayController::showKeyboard(QString existingText, unsigned long userValue) {
	vr::VROverlay()->ShowKeyboardForOverlay(m_ulOverlayHandle, vr::k_EGamepadTextInputModeNormal,
		vr::k_EGamepadTextInputLineModeSingleLine, false, showName, 1024, existingText.toStdString().c_str(), userValue);
}


void OverlayController::playActivationSound() {

}


void OverlayController::playFocusChangedSound() {

}


QString OverlayController::openvrButtonToString(unsigned deviceId, unsigned buttonId) {
	QString name;
	auto nameIt = _openVRButtonNames.find(buttonId);
	if (nameIt != _openVRButtonNames.end()) {
		name = nameIt->second;
	}
	else {
		name.append("Button_").append(QString::number(buttonId));
	}
	if (deviceId != vr::k_unTrackedDeviceIndexInvalid && buttonId >= vr::k_EButton_Axis0 && buttonId <= vr::k_EButton_Axis4) {
		name.append(" (");
		vr::ETrackedPropertyError pError;
		auto axisType = vr::VRSystem()->GetInt32TrackedDeviceProperty(deviceId, (vr::ETrackedDeviceProperty)((int)vr::Prop_Axis0Type_Int32 + (buttonId - (int)vr::k_EButton_Axis0)), &pError);
		if (pError == vr::TrackedProp_Success) {
			switch (axisType) {
			case vr::k_eControllerAxis_Trigger:
				name.append("Trigger)");
				break;
			case vr::k_eControllerAxis_TrackPad:
				name.append("TrackPad)");
				break;
			case vr::k_eControllerAxis_Joystick:
				name.append("Joystick)");
				break;
			default:
				name.append("<unknown>)");
				break;
			}
		}
	}
	return name;
}


QString OverlayController::openvrAxisToString(unsigned deviceId, unsigned axisId) {
	QString name("Axis");
	name.append(QString::number(axisId));
	if (deviceId != vr::k_unTrackedDeviceIndexInvalid) {
		vr::ETrackedPropertyError pError;
		auto axisType = vr::VRSystem()->GetInt32TrackedDeviceProperty(deviceId, (vr::ETrackedDeviceProperty)((int)vr::Prop_Axis0Type_Int32 + axisId), &pError);
		if (pError == vr::TrackedProp_Success && axisType != vr::k_eControllerAxis_None) {
			switch (axisType) {
			case vr::k_eControllerAxis_Trigger:
				name.append(" (Trigger)");
				break;
			case vr::k_eControllerAxis_TrackPad:
				name.append(" (TrackPad)");
				break;
			case vr::k_eControllerAxis_Joystick:
				name.append(" (Joystick)");
				break;
			default:
				name.append("<unknown>)");
				break;
			}
		}
	}
	return name;
}


unsigned OverlayController::keyboardVirtualCodeCount() {
	return (unsigned)_keyboardVirtualCodes.size();
}

QString OverlayController::keyboardVirtualCodeNameFromIndex(unsigned index) {
	if (index < _keyboardVirtualCodes.size()) {
		return QString::fromStdString(_keyboardVirtualCodes[index].first);
	}
	else {
		return QString();
	}
}

unsigned OverlayController::keyboardVirtualCodeIdFromIndex(unsigned index) {
	if (index < _keyboardVirtualCodes.size()) {
		return (unsigned)_keyboardVirtualCodes[index].second;
	}
	else {
		return 0;
	}
}

unsigned OverlayController::keyboardVirtualCodeIndexFromId(unsigned id) {
	unsigned index = 0;
	for (unsigned i = 0; i < _keyboardVirtualCodes.size(); i++) {
		if (id == _keyboardVirtualCodes[i].second) {
			index = i;
			break;
		}
	}
	return index;
}
