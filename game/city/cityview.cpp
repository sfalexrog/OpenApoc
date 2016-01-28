#include "library/sp.h"
#include "game/city/city.h"
#include "game/city/cityview.h"
#include "framework/framework.h"
#include "game/resources/gamecore.h"
#include "game/ufopaedia/ufopaedia.h"
#include "game/city/infiltrationscreen.h"
#include "game/city/scorescreen.h"
#include "game/general/ingameoptions.h"
#include "game/city/vehiclemission.h"
#include "game/city/vehicle.h"
#include "game/city/building.h"
#include "game/base/basescreen.h"
#include "game/base/vequipscreen.h"
#include "game/tileview/tileobject_vehicle.h"
#include "game/tileview/tileobject_scenery.h"
#include "game/tileview/voxel.h"
#include "game/city/scenery.h"
#include "game/city/buildingscreen.h"
#include "game/city/baseselectscreen.h"

namespace OpenApoc
{
namespace
{

static const std::map<CityIcon, UString> CITY_ICON_RESOURCES = {
    // FIXME: Put this in the rules somewhere?
    {CityIcon::UnselectedFrame,
     "PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:37:xcom3/UFODATA/PAL_01.DAT"},
    {CityIcon::SelectedFrame,
     "PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:39:xcom3/UFODATA/PAL_01.DAT"},
    {CityIcon::SelectedSecondaryFrame,
     "PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:38:xcom3/UFODATA/PAL_01.DAT"},

    {CityIcon::InBase,
     "PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:47:xcom3/UFODATA/PAL_01.DAT"},
    {CityIcon::InVehicle,
     "PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:48:xcom3/UFODATA/PAL_01.DAT"},
    {CityIcon::InBuilding,
     "PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:49:xcom3/UFODATA/PAL_01.DAT"},
    {CityIcon::InMotion,
     "PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:50:xcom3/UFODATA/PAL_01.DAT"},
};
static const std::vector<UString> TAB_FORM_NAMES = {
    "FORM_CITY_UI_1", "FORM_CITY_UI_2", "FORM_CITY_UI_3", "FORM_CITY_UI_4",
    "FORM_CITY_UI_5", "FORM_CITY_UI_6", "FORM_CITY_UI_7", "FORM_CITY_UI_8",
};

static const std::vector<UString> CITY_ICON_VEHICLE_PASSENGER_COUNT_RESOURCES = {
    {""}, // 0 has no image
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:51:xcom3/UFODATA/PAL_01.DAT"}, // 1
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:52:xcom3/UFODATA/PAL_01.DAT"}, // 2
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:53:xcom3/UFODATA/PAL_01.DAT"}, // 3
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:54:xcom3/UFODATA/PAL_01.DAT"}, // 4
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:55:xcom3/UFODATA/PAL_01.DAT"}, // 5
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:56:xcom3/UFODATA/PAL_01.DAT"}, // 6
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:57:xcom3/UFODATA/PAL_01.DAT"}, // 7
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:58:xcom3/UFODATA/PAL_01.DAT"}, // 8
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:59:xcom3/UFODATA/PAL_01.DAT"}, // 9
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:60:xcom3/UFODATA/PAL_01.DAT"}, // 10
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:61:xcom3/UFODATA/PAL_01.DAT"}, // 11
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:62:xcom3/UFODATA/PAL_01.DAT"}, // 12
    {"PCK:xcom3/UFODATA/VS_ICON.PCK:xcom3/UFODATA/VS_ICON.TAB:63:xcom3/UFODATA/PAL_01.DAT"}, // 13+
};

} // anonymous namespace

CityView::CityView(sp<GameState> state)
    : TileView(state->city->map, Vec3<int>{CITY_TILE_X, CITY_TILE_Y, CITY_TILE_Z},
               Vec2<int>{CITY_STRAT_TILE_X, CITY_STRAT_TILE_Y}, TileViewMode::Isometric),
      baseForm(fw().gamecore->GetForm("FORM_CITY_UI")), updateSpeed(UpdateSpeed::Speed1),
      state(state), followVehicle(false), selectionState(SelectionState::Normal)
{
	baseForm->FindControlTyped<RadioButton>("BUTTON_SPEED1")->SetChecked(true);
	for (auto &formName : TAB_FORM_NAMES)
	{
		sp<Form> f(fw().gamecore->GetForm(formName));
		if (!f)
		{
			LogError("Failed to load form \"%s\"", formName.c_str());
			return;
		}
		f->takesFocus = false;
		this->uiTabs.push_back(f);
	}
	this->activeTab = this->uiTabs[0];

	for (auto &base : state->playerBases)
	{
		auto bld = base->bld.lock();
		if (!bld)
		{
			LogError("Base with invalid bld");
		}
		auto bldBounds = bld->def.getBounds();

		Vec2<int> buildingCenter = (bldBounds.p0 + bldBounds.p1) / 2;
		this->setScreenCenterTile(buildingCenter);
		break;
	}

	for (auto &iconResource : CITY_ICON_RESOURCES)
	{
		auto &type = iconResource.first;
		auto &path = iconResource.second;
		auto image = fw().data->load_image(path);
		if (!image)
		{
			LogError("Failed to open city icon resource \"%s\"", path.c_str());
		}
		this->icons[type] = image;
	}

	for (auto &passengerResource : CITY_ICON_VEHICLE_PASSENGER_COUNT_RESOURCES)
	{
		auto image = fw().data->load_image(passengerResource);
		if (!image && passengerResource != "")
		{
			LogError("Failed to open city vehicle passenger icon resource \"%s\"",
			         passengerResource.c_str());
		}
		this->vehiclePassengerCountIcons.push_back(image);
	}

	auto img = mksp<RGBImage>(Vec2<int>{1, 2});
	{
		RGBImageLock l(img);
		l.set({0, 0}, Colour{255, 255, 219});
		l.set({0, 1}, Colour{215, 0, 0});
	}
	this->healthImage = img;
	img = mksp<RGBImage>(Vec2<int>{1, 2});
	{
		RGBImageLock l(img);
		l.set({0, 0}, Colour{160, 236, 252});
		l.set({0, 1}, Colour{4, 100, 252});
	}
	this->shieldImage = img;

	this->baseForm->FindControl("BUTTON_TAB_1")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[0];
		              });
	this->baseForm->FindControl("BUTTON_TAB_2")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[1];
		              });
	this->baseForm->FindControl("BUTTON_TAB_3")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[2];
		              });
	this->baseForm->FindControl("BUTTON_TAB_4")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[3];
		              });
	this->baseForm->FindControl("BUTTON_TAB_5")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[4];
		              });
	this->baseForm->FindControl("BUTTON_TAB_6")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[5];
		              });
	this->baseForm->FindControl("BUTTON_TAB_7")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[6];
		              });
	this->baseForm->FindControl("BUTTON_TAB_8")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->activeTab = this->uiTabs[7];
		              });
	this->baseForm->FindControl("BUTTON_FOLLOW_VEHICLE")
	    ->addCallback(FormEventType::CheckBoxChange, [this](Event *e) -> void
	                  {
		                  this->followVehicle =
		                      std::dynamic_pointer_cast<CheckBox>(e->Forms().RaisedBy)->IsChecked();
		              });
	this->baseForm->FindControl("BUTTON_TOGGLE_STRATMAP")
	    ->addCallback(FormEventType::CheckBoxChange, [this](Event *e) -> void
	                  {
		                  bool strategy =
		                      std::dynamic_pointer_cast<CheckBox>(e->Forms().RaisedBy)->IsChecked();
		                  this->setViewMode(strategy ? TileViewMode::Strategy
		                                             : TileViewMode::Isometric);
		              });
	this->baseForm->FindControl("BUTTON_SPEED0")
	    ->addCallback(FormEventType::CheckBoxSelected, [this](Event *e) -> void
	                  {
		                  this->updateSpeed = UpdateSpeed::Pause;
		              });
	this->baseForm->FindControl("BUTTON_SPEED1")
	    ->addCallback(FormEventType::CheckBoxSelected, [this](Event *e) -> void
	                  {
		                  this->updateSpeed = UpdateSpeed::Speed1;
		              });
	this->baseForm->FindControl("BUTTON_SPEED2")
	    ->addCallback(FormEventType::CheckBoxSelected, [this](Event *e) -> void
	                  {
		                  this->updateSpeed = UpdateSpeed::Speed2;
		              });
	this->baseForm->FindControl("BUTTON_SPEED3")
	    ->addCallback(FormEventType::CheckBoxSelected, [this](Event *e) -> void
	                  {
		                  this->updateSpeed = UpdateSpeed::Speed3;
		              });
	this->baseForm->FindControl("BUTTON_SPEED4")
	    ->addCallback(FormEventType::CheckBoxSelected, [this](Event *e) -> void
	                  {
		                  this->updateSpeed = UpdateSpeed::Speed4;
		              });
	this->baseForm->FindControl("BUTTON_SPEED5")
	    ->addCallback(FormEventType::CheckBoxSelected, [this](Event *e) -> void
	                  {
		                  this->updateSpeed = UpdateSpeed::Speed5;
		              });
	this->baseForm->FindControl("BUTTON_SHOW_ALIEN_INFILTRATION")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->stageCmd.cmd = StageCmd::Command::PUSH;
		                  this->stageCmd.nextStage = mksp<InfiltrationScreen>();
		              });
	this->baseForm->FindControl("BUTTON_SHOW_SCORE")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->stageCmd.cmd = StageCmd::Command::PUSH;
		                  this->stageCmd.nextStage = mksp<ScoreScreen>();
		              });
	this->baseForm->FindControl("BUTTON_SHOW_UFOPAEDIA")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->stageCmd.cmd = StageCmd::Command::PUSH;
		                  this->stageCmd.nextStage = mksp<Ufopaedia>();
		              });
	this->baseForm->FindControl("BUTTON_SHOW_OPTIONS")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->stageCmd.cmd = StageCmd::Command::PUSH;
		                  this->stageCmd.nextStage = mksp<InGameOptions>(this->state);
		              });
	this->baseForm->FindControl("BUTTON_SHOW_LOG")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  LogWarning("Show log");
		              });
	this->baseForm->FindControl("BUTTON_ZOOM_EVENT")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  LogWarning("Zoom to event");
		              });

	auto baseManagementForm = this->uiTabs[0];
	baseManagementForm->FindControl("BUTTON_SHOW_BASE")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->stageCmd.cmd = StageCmd::Command::PUSH;
		                  this->stageCmd.nextStage = mksp<BaseScreen>(this->state);
		              });
	baseManagementForm->FindControl("BUTTON_BUILD_BASE")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->stageCmd.cmd = StageCmd::Command::PUSH;
		                  this->stageCmd.nextStage = mksp<BaseSelectScreen>();
		              });
	auto vehicleForm = this->uiTabs[1];
	vehicleForm->FindControl("BUTTON_EQUIP_VEHICLE")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  this->stageCmd.cmd = StageCmd::Command::PUSH;
		                  auto equipScreen = mksp<VEquipScreen>(this->state);
		                  auto v = this->selectedVehicle.lock();
		                  if (v && v->owner == this->state->getPlayer())
		                  {
			                  equipScreen->setSelectedVehicle(v);
		                  }
		                  this->stageCmd.nextStage = equipScreen;
		              });
	vehicleForm->FindControl("BUTTON_VEHICLE_BUILDING")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  auto v = this->selectedVehicle.lock();
		                  if (v && v->owner == this->state->getPlayer())
		                  {
			                  auto b = v->building.lock();
			                  if (b)
			                  {
				                  this->stageCmd.cmd = StageCmd::Command::PUSH;
				                  this->stageCmd.nextStage = mksp<BuildingScreen>(b);
			                  }
		                  }
		              });
	vehicleForm->FindControl("BUTTON_GOTO_BUILDING")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  auto v = this->selectedVehicle.lock();
		                  if (v && v->owner == this->state->getPlayer())
		                  {
			                  LogInfo("Select building for vehicle \"%s\"", v->name.c_str());
			                  this->selectionState = SelectionState::VehicleGotoBuilding;
		                  }

		              });
	vehicleForm->FindControl("BUTTON_GOTO_LOCATION")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  auto v = this->selectedVehicle.lock();
		                  if (v && v->owner == this->state->getPlayer())
		                  {
			                  LogInfo("Select building for vehicle \"%s\"", v->name.c_str());
			                  this->selectionState = SelectionState::VehicleGotoLocation;
		                  }

		              });
	vehicleForm->FindControl("BUTTON_GOTO_BASE")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  auto v = this->selectedVehicle.lock();
		                  if (v && v->owner == this->state->getPlayer())
		                  {
			                  LogWarning("Goto base for vehicle \"%s\"", v->name.c_str());
			                  auto base = v->homeBase.lock();
			                  if (!base)
			                  {
				                  LogError("Vehicle \"%s\" has no home base", v->name.c_str());
			                  }
			                  auto bld = base->bld.lock();
			                  if (!bld)
			                  {
				                  LogError("Base \"%s\" has no building", base->name.c_str());
			                  }
			                  LogWarning("Vehicle \"%s\" goto building \"%s\"", v->name.c_str(),
			                             bld->def.getName().c_str());
			                  // FIXME: Don't clear missions if not replacing current mission
			                  v->missions.clear();
			                  v->missions.emplace_back(
			                      VehicleMission::gotoBuilding(*v, this->state->city->map, bld));
			                  v->missions.front()->start();
		                  }
		              });
	vehicleForm->FindControl("BUTTON_VEHICLE_ATTACK")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  auto v = this->selectedVehicle.lock();
		                  if (v && v->owner == this->state->getPlayer())
		                  {
			                  LogInfo("Select target for vehicle \"%s\"", v->name.c_str());
			                  this->selectionState = SelectionState::VehicleGotoLocation;
		                  }

		              });
	vehicleForm->FindControl("BUTTON_VEHICLE_ATTACK_BUILDING")
	    ->addCallback(FormEventType::ButtonClick, [this](Event *e) -> void
	                  {
		                  auto v = this->selectedVehicle.lock();
		                  if (v && v->owner == this->state->getPlayer())
		                  {
			                  LogInfo("Select target building for vehicle \"%s\"", v->name.c_str());
			                  this->selectionState = SelectionState::VehicleGotoLocation;
		                  }

		              });
}

CityView::~CityView() {}

void CityView::Render()
{
	TileView::Render();
	if (state->showVehiclePath)
	{
		for (auto v : state->city->vehicles)
		{
			auto vTile = v->tileObject;
			if (!vTile)
				continue;
			auto &path = v->missions.front()->getCurrentPlannedPath();
			Vec3<float> prevPos = vTile->getPosition();
			for (auto *tile : path)
			{
				auto screenOffset = this->getScreenOffset();
				Vec3<float> pos = tile->position;
				Vec2<float> screenPosA = this->tileToScreenCoords(prevPos);
				screenPosA += screenOffset;
				Vec2<float> screenPosB = this->tileToScreenCoords(pos);
				screenPosB += screenOffset;

				fw().renderer->drawLine(screenPosA, screenPosB, Colour{255, 0, 0, 128});

				prevPos = pos;
			}
		}
	}
	activeTab->Render();
	baseForm->Render();
	fw().gamecore->MouseCursor->Render();
}

void CityView::Update(StageCmd *const cmd)
{
	unsigned int ticks = 0;
	switch (this->updateSpeed)
	{
		case UpdateSpeed::Pause:
			ticks = 0;
			break;
		/* POSSIBLE FIXME: 'vanilla' apoc appears to implement Speed1 as 1/2 speed - that is only
		 * every other call calls the update loop, meaning that the later update tick counts are
		 * halved as well.
		 * This effectively means that all openapoc tick counts count for 1/2 the value of vanilla
		 * apoc ticks */
		case UpdateSpeed::Speed1:
			ticks = 1;
			break;
		case UpdateSpeed::Speed2:
			ticks = 2;
			break;
		case UpdateSpeed::Speed3:
			ticks = 4;
			break;
		case UpdateSpeed::Speed4:
			ticks = 6;
			break;
		case UpdateSpeed::Speed5:
			LogWarning("FIXME: Sort out higher speed to not just hammer update (GOD-SLOW) - "
			           "demoting to speed4");
			/* FIXME: This should be '600' according to the scale above? */
			ticks = 6;
			this->updateSpeed = UpdateSpeed::Speed4;
			break;
	}

	// As the city view is the only screen that actually progresses time, incremnt that here
	state->time += ticks;

	auto clockControl = baseForm->FindControlTyped<Label>("CLOCK");

	auto seconds = state->time / TICKS_PER_SECOND;
	auto minutes = seconds / 60;
	auto hours = minutes / 60;

	unsigned secondsClamped = seconds % 60;
	unsigned minutesClamped = minutes % 60;
	unsigned hoursClamped = hours % 24;

	auto timeString =
	    UString::format("%02u:%02u:%02u", hoursClamped, minutesClamped, secondsClamped);
	clockControl->SetText(timeString);

	*cmd = stageCmd;
	stageCmd = StageCmd();

	state->city->update(*state, ticks);

	// FIXME: Possibly more efficient ways than re-generating all controls every frame?

	// Setup owned vehicle list controls
	auto ownedVehicleList = uiTabs[1]->FindControlTyped<ListBox>("OWNED_VEHICLE_LIST");
	if (!ownedVehicleList)
	{
		LogError("Failed to find \"OWNED_VEHICLE_LIST\" control on city tab \"%s\"",
		         TAB_FORM_NAMES[1].c_str());
	}

	ownedVehicleList->ItemSpacing = 0;

	auto selected = this->selectedVehicle.lock();
	std::map<sp<Vehicle>, std::pair<VehicleTileInfo, sp<Control>>> newVehicleListControls;

	ownedVehicleList->Clear();

	if (activeTab == uiTabs[1])
	{
		for (auto &v : state->getPlayer()->vehicles)
		{
			auto vehicle = v.lock();
			if (!vehicle)
			{
				continue;
			}
			auto info = this->createVehicleInfo(vehicle);
			auto it = this->vehicleListControls.find(vehicle);
			sp<Control> control;
			if (it != this->vehicleListControls.end() && it->second.first == info)
			{
				// Control unchanged, don't regenerate
				control = it->second.second;
			}
			else
			{
				control = createVehicleInfoControl(info);
			}
			newVehicleListControls[vehicle] = std::make_pair(info, control);
			ownedVehicleList->AddItem(control);

			control->addCallback(FormEventType::MouseDown, [this, vehicle](Event *e) -> void
			                     {
				                     this->selectedVehicle = vehicle;
				                 });
		}
	}

	// Clear the old list and reset to the new one (May be empty is not on a vehicle-displaying tab
	this->vehicleListControls.clear();
	this->vehicleListControls = std::move(newVehicleListControls);

	activeTab->Update();
	baseForm->Update();

	// If we have 'follow vehicle' enabled we clobber any other movement that may have occurred in
	// this frame
	if (this->followVehicle)
	{
		auto v = this->selectedVehicle.lock();
		if (v)
		{
			// The selected vehicle may not have a tile object if it's not on the map
			if (v->tileObject)
			{
				this->setScreenCenterTile(v->tileObject->getPosition());
			}
		}
	}
}

void CityView::EventOccurred(Event *e)
{
	// FIXME: There should be a better place for handling "clicks"
	static bool isTouched = false;
	static int xTouch, yTouch;
	const int thresholdX = 10;
	const int thresholdY = 10;

	fw().gamecore->MouseCursor->EventOccured(e);
	activeTab->EventOccured(e);
	baseForm->EventOccured(e);

	if (activeTab->eventIsWithin(e) || baseForm->eventIsWithin(e))
	{
		return;
	}

	if (e->Type() == EVENT_KEY_DOWN && e->Keyboard().KeyCode == SDLK_ESCAPE)
	{
		stageCmd.cmd = StageCmd::Command::POP;
		return;
	}
	// FIXME: Check if scancode is better/worse
	else if (e->Type() == EVENT_KEY_DOWN &&
	         SDL_GetScancodeFromKey(e->Keyboard().KeyCode) == SDL_SCANCODE_R)
	{
		LogInfo("Repairing...");
		std::set<sp<Scenery>> stuffToRepair;
		for (auto &s : state->city->scenery)
		{
			if (s->canRepair())
			{
				stuffToRepair.insert(s);
			}
		}
		LogInfo("Repairing %u tiles out of %u", static_cast<unsigned>(stuffToRepair.size()),
		        static_cast<unsigned>(state->city->scenery.size()));

		for (auto &s : stuffToRepair)
		{
			s->repair(*state);
			state->city->fallingScenery.erase(s);
		}
	}
	else if (e->Type() == EVENT_FINGER_DOWN)
	{
		isTouched = true;
		xTouch = e->Finger().X;
		yTouch = e->Finger().Y;
	}
	else if (e->Type() == EVENT_FINGER_MOVE)
	{
		int x = e->Finger().X;
		int y = e->Finger().Y;
		bool overThreshold = (abs(x - xTouch) > thresholdX) && (abs(y - yTouch) > thresholdY);
		if (overThreshold)
		{
			isTouched = false;
			TileView::EventOccurred(e);
		}
	}
	else if (e->Type() == EVENT_FINGER_UP)
	{
		if (isTouched)
		{
			MouseEvent mdEvent(EVENT_MOUSE_DOWN);
			mdEvent.Mouse().X = e->Finger().X;
			mdEvent.Mouse().Y = e->Finger().Y;
			mdEvent.Mouse().DeltaX = e->Finger().DeltaX;
			mdEvent.Mouse().DeltaY = e->Finger().DeltaY;
			mdEvent.Mouse().WheelVertical = 0;
			mdEvent.Mouse().WheelHorizontal = 0;
			mdEvent.Mouse().Button = 1;
			CityView::EventOccurred(&mdEvent);
		}
		isTouched = false;
	}
	// Exclude mouse down events that are over the form
	else if (e->Type() == EVENT_MOUSE_DOWN)
	{
		if (this->getViewMode() == TileViewMode::Strategy && e->Type() == EVENT_MOUSE_DOWN &&
		    e->Mouse().Button == 2)
		{
			Vec2<float> screenOffset = {this->getScreenOffset().x, this->getScreenOffset().y};
			auto clickTile = this->screenToTileCoords(
			    Vec2<float>{e->Mouse().X, e->Mouse().Y} - screenOffset, 0.0f);
			this->setScreenCenterTile({clickTile.x, clickTile.y});
		}
		else if (e->Type() == EVENT_MOUSE_DOWN && e->Mouse().Button == 1)
		{
			// If a click has not been handled by a form it's in the map. See if we intersect with
			// anything
			Vec2<float> screenOffset = {this->getScreenOffset().x, this->getScreenOffset().y};
			auto clickTop = this->screenToTileCoords(
			    Vec2<float>{e->Mouse().X, e->Mouse().Y} - screenOffset, 9.99f);
			auto clickBottom = this->screenToTileCoords(
			    Vec2<float>{e->Mouse().X, e->Mouse().Y} - screenOffset, 0.0f);
			auto collision = state->city->map.findCollision(clickTop, clickBottom);
			if (collision)
			{
				if (collision.obj->getType() == TileObject::Type::Scenery)
				{
					auto scenery =
					    std::dynamic_pointer_cast<TileObjectScenery>(collision.obj)->getOwner();
					LogInfo("Clicked on scenery at {%d,%d,%d}", scenery->pos.x, scenery->pos.y,
					        scenery->pos.z);
					if (this->selectionState == SelectionState::VehicleGotoLocation)
					{

						auto v = this->selectedVehicle.lock();
						if (v && v->owner == state->getPlayer())
						{
							Vec3<int> targetPos{scenery->pos.x, scenery->pos.y,
							                    state->city->map.size.z - 1};
							// FIXME: Use vehicle 'height' hint to select target height?
							// FIXME: Don't clear missions if not replacing current mission
							v->missions.clear();
							v->missions.emplace_back(
							    VehicleMission::gotoLocation(*v, state->city->map, targetPos));
							v->missions.front()->start();
							LogWarning("Vehicle \"%s\" going to location {%d,%d,%d}",
							           v->name.c_str(), targetPos.x, targetPos.y, targetPos.z);
						}
						this->selectionState = SelectionState::Normal;
					}
					auto building = scenery->building;
					if (building)
					{
						LogInfo("Scenery owned by building \"%s\"",
						        building->def.getName().c_str());
						if (this->selectionState == SelectionState::VehicleGotoBuilding)
						{
							auto v = this->selectedVehicle.lock();
							if (v && v->owner == state->getPlayer())
							{
								LogWarning("Vehicle \"%s\" goto building \"%s\"", v->name.c_str(),
								           building->def.getName().c_str());
								// FIXME: Don't clear missions if not replacing current mission
								v->missions.clear();
								v->missions.emplace_back(
								    VehicleMission::gotoBuilding(*v, state->city->map, building));
								v->missions.front()->start();
							}
							this->selectionState = SelectionState::Normal;
						}
						else if (this->selectionState == SelectionState::VehicleAttackBuilding)
						{
							auto v = this->selectedVehicle.lock();
							if (v)
							{
								// TODO: Attack building mission
								LogWarning("Vehicle \"%s\" attack building \"%s\"", v->name.c_str(),
								           building->def.getName().c_str());
							}
							this->selectionState = SelectionState::Normal;
						}
						else if (this->selectionState == SelectionState::Normal)
						{
							stageCmd.cmd = StageCmd::Command::PUSH;
							stageCmd.nextStage = mksp<BuildingScreen>(building);
						}

						return;
					}
				}
				else if (collision.obj->getType() == TileObject::Type::Vehicle)
				{
					auto vehicle =
					    std::dynamic_pointer_cast<TileObjectVehicle>(collision.obj)->getVehicle();
					LogWarning("Clicked on vehicle \"%s\"", vehicle->name.c_str());
				}
			}
		}
	}
	else
	{
		TileView::EventOccurred(e);
	}
}

VehicleTileInfo CityView::createVehicleInfo(sp<Vehicle> v)
{
	VehicleTileInfo t;
	t.vehicle = v;
	t.selected = (v == this->selectedVehicle.lock());

	float maxHealth;
	float currentHealth;
	if (v->getShield() != 0)
	{
		maxHealth = v->getMaxShield();
		currentHealth = v->getShield();
		t.shield = true;
	}
	else
	{
		maxHealth = v->getMaxHealth();
		currentHealth = v->getHealth();
		t.shield = false;
	}

	t.healthProportion = currentHealth / maxHealth;
	// Clamp passengers to 13 as anything beyond that gets the same icon
	t.passengers = std::min(13, v->getPassengers());
	// FIXME Fade out vehicles that are on the way to/back from the alien dimension
	t.faded = false;

	auto b = v->building.lock();
	if (b)
	{
		if (b->base == v->homeBase.lock())
		{
			t.state = CityUnitState::InBase;
		}
		else
		{
			t.state = CityUnitState::InBuilding;
		}
	}
	else
	{
		t.state = CityUnitState::InMotion;
	}
	return t;
}

sp<Control> CityView::createVehicleInfoControl(const VehicleTileInfo &info)
{
	LogInfo("Creating city info control for vehicle \"%s\"", info.vehicle->name.c_str());

	auto frame = info.selected ? this->icons[CityIcon::SelectedFrame]
	                           : this->icons[CityIcon::UnselectedFrame];
	auto baseControl = mksp<GraphicButton>(frame, frame);
	baseControl->Size = frame->size;
	// FIXME: There's an extra 1 pixel here that's annoying
	baseControl->Size.x -= 1;
	baseControl->Name = "OWNED_VEHICLE_FRAME_" + info.vehicle->name;

	auto vehicleIcon = baseControl->createChild<Graphic>(info.vehicle->type.icon);
	vehicleIcon->AutoSize = true;
	vehicleIcon->Location = {1, 1};
	vehicleIcon->Name = "OWNED_VEHICLE_ICON_" + info.vehicle->name;

	// FIXME: Put these somewhere slightly less magic?
	Vec2<int> healthBarOffset = {27, 2};
	Vec2<int> healthBarSize = {3, 20};

	auto healthImg = info.shield ? this->shieldImage : this->healthImage;

	auto healthGraphic = baseControl->createChild<Graphic>(healthImg);
	// This is a bit annoying as the health bar starts at the bottom, but the coord origin is
	// top-left, so fix that up a bit
	int healthBarHeight = (float)healthBarSize.y * info.healthProportion;
	healthBarOffset.y = healthBarOffset.y + (healthBarSize.y - healthBarHeight);
	healthBarSize.y = healthBarHeight;
	healthGraphic->Location = healthBarOffset;
	healthGraphic->Size = healthBarSize;
	healthGraphic->ImagePosition = FillMethod::Stretch;

	sp<Graphic> stateGraphic;

	switch (info.state)
	{
		case CityUnitState::InBase:
			stateGraphic = vehicleIcon->createChild<Graphic>(this->icons[CityIcon::InBase]);
			break;
		case CityUnitState::InVehicle:
			stateGraphic = vehicleIcon->createChild<Graphic>(this->icons[CityIcon::InVehicle]);
			break;
		case CityUnitState::InBuilding:
			stateGraphic = vehicleIcon->createChild<Graphic>(this->icons[CityIcon::InBuilding]);
			break;
		case CityUnitState::InMotion:
			stateGraphic = vehicleIcon->createChild<Graphic>(this->icons[CityIcon::InMotion]);
			break;
	}

	stateGraphic->AutoSize = true;
	stateGraphic->Location = {0, 0};
	stateGraphic->Name = "OWNED_VEHICLE_STATE_" + info.vehicle->name;

	if (info.passengers)
	{
		auto passengerGraphic =
		    vehicleIcon->createChild<Graphic>(this->vehiclePassengerCountIcons[info.passengers]);
		passengerGraphic->AutoSize = true;
		passengerGraphic->Location = {0, 0};
		passengerGraphic->Name = "OWNED_VEHICLE_PASSENGERS_" + info.vehicle->name;
	}

	return baseControl;
}

bool VehicleTileInfo::operator==(const VehicleTileInfo &other) const
{
	return (this->vehicle == other.vehicle && this->selected == other.selected &&
	        this->healthProportion == other.healthProportion && this->shield == other.shield &&
	        this->passengers == other.passengers && this->state == other.state);
}

}; // namespace OpenApoc
