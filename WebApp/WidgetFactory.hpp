#pragma once

#include <memory>
#include <Wt/WPushButton.h>
#include <Wt/WTable.h>

std::unique_ptr<Wt::WPushButton> createButton(const std::string& text, std::function<void()> handler);
std::unique_ptr<Wt::WTable> createList();