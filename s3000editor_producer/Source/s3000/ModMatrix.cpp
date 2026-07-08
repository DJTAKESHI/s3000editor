#include "ModMatrix.h"

//float ModMatrix::evaluate(const ModContext& ctx, ModDestination target) const
//{
//	float sum = 0.0f;
//
//	for (const auto& slot : slots)
//	{
//		if (slot.source == ModSource::Off)
//		{
//			continue;
//		}
//
//		if (slot.dest != target)
//		{
//			continue;
//		}
//
//		float src = ctx.get(slot.source);
//		float amt = slot.normalized();
//
//		sum += src * amt;
//	}
//
//	return sum;
//}
//
//void ModMatrix::clear()
//{
//	for (auto& s : slots)
//	{
//		s.source = ModSource::Off;
//		s.dest = ModDestination::None;
//		s.amount = 0;
//	}
//
//}