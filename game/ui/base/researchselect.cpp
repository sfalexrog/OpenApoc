#include "game/ui/base/researchselect.h"
#include "forms/form.h"
#include "forms/graphic.h"
#include "forms/graphicbutton.h"
#include "forms/label.h"
#include "forms/list.h"
#include "forms/ui.h"
#include "framework/event.h"
#include "framework/framework.h"
#include "framework/keycodes.h"
#include "game/state/base/base.h"
#include "game/state/gamestate.h"
#include "game/state/organisation.h"
#include "game/state/research.h"
#include "game/ui/general/messagebox.h"
#include "library/strings_format.h"

namespace OpenApoc
{

ResearchSelect::ResearchSelect(sp<GameState> state, sp<Lab> lab)
    : Stage(), form(ui().getForm("researchselect")), lab(lab), state(state)
{
}

ResearchSelect::~ResearchSelect() = default;

void ResearchSelect::begin()
{
	current_topic = this->lab->current_project;

	form->findControlTyped<Label>("TEXT_FUNDS")->setText(state->getPlayerBalance());
	auto title = form->findControlTyped<Label>("TEXT_TITLE");
	auto progress = form->findControlTyped<Label>("TEXT_PROGRESS");
	auto skill = form->findControlTyped<Label>("TEXT_SKILL");
	switch (this->lab->type)
	{
		case ResearchTopic::Type::BioChem:
			title->setText(tr("Select Biochemistry Project"));
			progress->setText(tr("Progress"));
			skill->setText(tr("Skill"));
			break;
		case ResearchTopic::Type::Physics:
			title->setText(tr("Select Physics Project"));
			progress->setText(tr("Progress"));
			skill->setText(tr("Skill"));
			break;
		case ResearchTopic::Type::Engineering:
			title->setText(tr("Select Manufacturing Project"));
			progress->setText(tr("Unit Cost"));
			skill->setText(tr("Skill Hours"));
			break;
		default:
			title->setText(tr("Select Unknown Project"));
			break;
	}
	this->populateResearchList();
	this->redrawResearchList();

	auto research_list = form->findControlTyped<ListBox>("LIST");
	research_list->AlwaysEmitSelectionEvents = true;

	research_list->addCallback(FormEventType::ListBoxChangeSelected, [this](FormsEvent *e) {
		LogInfo("Research selection change");
		auto list = std::static_pointer_cast<ListBox>(e->forms().RaisedBy);
		auto topic = list->getSelectedData<ResearchTopic>();
		if (topic->current_lab)
		{
			LogInfo("Topic already in progress");
			return;
		}
		if (topic->isComplete())
		{
			LogInfo("Topic already complete");
			auto message_box =
			    mksp<MessageBox>(tr("PROJECT COMPLETE"), tr("This project is already complete."),
			                     MessageBox::ButtonOptions::Ok);
			fw().stageQueueCommand({StageCmd::Command::PUSH, message_box});
			return;
		}
		if (topic->required_lab_size == ResearchTopic::LabSize::Large &&
		    this->lab->size == ResearchTopic::LabSize::Small)
		{
			LogInfo("Topic is large and lab is small");
			auto message_box = mksp<MessageBox>(
			    tr("PROJECT TOO LARGE"), tr("This project requires an advanced lab or workshop."),
			    MessageBox::ButtonOptions::Ok);
			fw().stageQueueCommand({StageCmd::Command::PUSH, message_box});
			return;
		}
		if (this->lab->type == ResearchTopic::Type::Engineering &&
		    topic->cost > state->player->balance)
		{
			LogInfo("Cannot afford to manufacture");
			auto message_box = mksp<MessageBox>(tr("FUNDS EXCEEDED"),
			                                    tr("Production costs exceed your available funds."),
			                                    MessageBox::ButtonOptions::Ok);
			fw().stageQueueCommand({StageCmd::Command::PUSH, message_box});
			return;
		}
		current_topic = topic;
		this->redrawResearchList();
	});

	research_list->addCallback(FormEventType::ListBoxChangeHover, [this](FormsEvent *e) {
		LogInfo("Research display on hover change");
		auto list = std::static_pointer_cast<ListBox>(e->forms().RaisedBy);
		auto topic = list->getHoveredData<ResearchTopic>();
		auto title = this->form->findControlTyped<Label>("TEXT_SELECTED_TITLE");
		auto description = this->form->findControlTyped<Label>("TEXT_SELECTED_DESCRIPTION");
		if (topic)
		{
			title->setText(tr(topic->name));
			description->setText(tr(topic->description));
		}
		else
		{
			title->setText("");
			description->setText("");
		}
	});

	auto ok_button = form->findControlTyped<GraphicButton>("BUTTON_OK");
	ok_button->addCallback(FormEventType::ButtonClick, [this](FormsEvent *) {
		LogInfo("Research selection OK pressed, applying selection");
		Lab::setResearch({state.get(), this->lab}, {state.get(), current_topic}, state);
	});
}

void ResearchSelect::redrawResearchList()
{
	for (auto &pair : control_map)
	{
		if (current_topic == pair.first)
		{
			pair.second->BackgroundColour = {127, 0, 0, 255};
		}
		else
		{
			pair.second->BackgroundColour = {0, 0, 0, 0};
		}
	}
}

void ResearchSelect::populateResearchList()
{
	auto research_list = form->findControlTyped<ListBox>("LIST");
	research_list->clear();
	research_list->ItemSize = 20;
	research_list->ItemSpacing = 1;

	for (auto &t : state->research.topic_list)
	{
		if (t->type != this->lab->type)
		{
			continue;
		}
		if (!t->dependencies.satisfied(state->current_base) && t->started == false)
		{
			continue;
		}
		// FIXME: When we get font coloring, set light blue color for topics too large a size
		bool too_large = (t->required_lab_size == ResearchTopic::LabSize::Large &&
		                  this->lab->size == ResearchTopic::LabSize::Small);
		std::ignore = too_large;

		auto control = mksp<Control>();
		control->Size = {544, 20};

		auto topic_name = control->createChild<Label>((t->name), ui().getFont("smalfont"));
		topic_name->Size = {200, 20};
		topic_name->Location = {6, 0};

		if (this->lab->type == ResearchTopic::Type::Engineering ||
		    ((this->lab->type == ResearchTopic::Type::BioChem ||
		      this->lab->type == ResearchTopic::Type::Physics) &&
		     t->isComplete()))
		{
			UString progress_text;
			if (this->lab->type == ResearchTopic::Type::Engineering)
				progress_text = format("$%d", t->cost);
			else
				progress_text = tr("Complete");
			auto progress_label =
			    control->createChild<Label>(progress_text, ui().getFont("smalfont"));
			progress_label->Size = {100, 20};
			progress_label->Location = {234, 0};
		}
		else
		{
			float projectProgress =
			    clamp((float)t->man_hours_progress / (float)t->man_hours, 0.0f, 1.0f);

			auto progressBar = control->createChild<Graphic>();
			progressBar->Size = {101, 6};
			progressBar->Location = {234, 4};

			auto progressImage = mksp<RGBImage>(progressBar->Size);
			int redWidth = progressBar->Size.x * projectProgress;
			{
				// FIXME: For some reason, there's no border here like in the research sceen, so we
				// have to make one manually, probably there's a better way
				RGBImageLock l(progressImage);
				for (int y = 0; y < 2; y++)
				{
					for (int x = 0; x < progressBar->Size.x; x++)
					{
						if (x < redWidth)
							l.set({x, y}, {255, 0, 0, 255});
						else
							l.set({x, y}, {77, 77, 77, 255});
					}
				}
				l.set({0, 2}, {77, 77, 77, 255});
				l.set({progressBar->Size.x - 1, 2}, {77, 77, 77, 255});
				l.set({0, 3}, {130, 130, 130, 255});
				l.set({progressBar->Size.x - 1, 3}, {130, 130, 130, 255});
				l.set({0, 4}, {140, 140, 140, 255});
				l.set({progressBar->Size.x - 1, 4}, {140, 140, 140, 255});
				for (int x = 0; x < progressBar->Size.x; x++)
				{
					l.set({x, 5}, {205, 205, 205, 255});
				}
			}
			progressBar->setImage(progressImage);
		}

		int skill_total = 0;
		switch (this->lab->type)
		{
			case ResearchTopic::Type::BioChem:
			case ResearchTopic::Type::Physics:
				if (t->current_lab)
					skill_total = t->current_lab->getTotalSkill();
				break;
			case ResearchTopic::Type::Engineering:
				skill_total = t->man_hours;
				break;
			default:
				break;
		}

		auto skill_total_label =
		    control->createChild<Label>(format("%d", skill_total), ui().getFont("smalfont"));
		skill_total_label->Size = {50, 20};
		skill_total_label->Location = {328, 0};
		skill_total_label->TextHAlign = HorizontalAlignment::Right;

		UString labSize;
		switch (t->required_lab_size)
		{
			case ResearchTopic::LabSize::Small:
				labSize = tr("Small");
				break;
			case ResearchTopic::LabSize::Large:
				labSize = tr("Large");
				break;
			default:
				labSize = tr("UNKNOWN");
				break;
		}

		auto lab_size_label = control->createChild<Label>(labSize, ui().getFont("smalfont"));
		lab_size_label->Size = {100, 20};
		lab_size_label->Location = {439, 0};

		control->setData(t);

		research_list->addItem(control);
		control_map[t] = control;
	}
}

void ResearchSelect::pause() {}

void ResearchSelect::resume() {}

void ResearchSelect::finish() {}

void ResearchSelect::eventOccurred(Event *e)
{
	form->eventOccured(e);

	if (e->type() == EVENT_KEY_DOWN)
	{
		if (e->keyboard().KeyCode == SDLK_ESCAPE)
		{
			fw().stageQueueCommand({StageCmd::Command::POP});
			return;
		}
	}

	if (e->type() == EVENT_FORM_INTERACTION)
	{
		if (e->forms().EventFlag == FormEventType::ButtonClick)
		{
			if (e->forms().RaisedBy->Name == "BUTTON_OK")
			{
				fw().stageQueueCommand({StageCmd::Command::POP});
				return;
			}
		}
	}
}

void ResearchSelect::update() { form->update(); }

void ResearchSelect::render()
{
	fw().stageGetPrevious(this->shared_from_this())->render();
	form->render();
}

bool ResearchSelect::isTransition() { return false; }

}; // namespace OpenApoc
