#pragma once

namespace xg::command
{
	/// The command has reached the end of the history and has "expired"
	/// If this is present on a command we know it is in the executed state
	/// Note this does not get added to commands that are severed from the history due to
	/// an undo and then adding new commands (see SeveredFromHistoryComponent)
	struct ExpiredFromHistoryComponent
	{
	};
}
