#pragma once

namespace xg::command
{
	/// The command has been undone and then replaced by another command
	/// If this is present on a command we know it is not currently executed
	struct SeveredFromHistoryComponent
	{
	};
}
