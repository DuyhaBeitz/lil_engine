#include "EditorUI.hpp"

Lil::RowSpacingGuard::RowSpacingGuard() { UIStyle::PushRowSpacing(); }
Lil::RowSpacingGuard::~RowSpacingGuard() { UIStyle::PopRowSpacing(); }
