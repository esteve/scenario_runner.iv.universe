#include <condition_plugins/acceleration_condition.h>

namespace condition_plugins
{

AccelerationCondition::AccelerationCondition()
  : scenario_conditions::ConditionBase {"Acceleration"}
{}

bool AccelerationCondition::configure(
  YAML::Node node,
  std::shared_ptr<ScenarioAPI> api_ptr)
try
{
  node_ = node;

  api_ptr_ = api_ptr;

  name_ = read_optional<std::string>(node_, "Name", name_);

  trigger_ = read_essential<std::string>(node_, "Trigger");

  value_ = read_essential<float>(node_, "Value");

  if (not parseRule<float>(read_essential<std::string>(node_, "Rule"), compare_))
  {
    return configured_ = false;
  }

  keep_ = read_optional<bool>(node_, "Keep", false);

  return configured_ = true;
}
catch (...)
{
  configured_ = false;
  SCENARIO_RETHROW_ERROR_FROM_CONDITION_CONFIGURATION();
}

bool AccelerationCondition::update(
  const std::shared_ptr<scenario_intersection::IntersectionManager> &)
{
  if (keep_ && result_)
  {
    return result_;
  }
  else
  {
    if ((*api_ptr_).isEgoCarName(trigger_))
    {
      return result_ = compare_((*api_ptr_).getAccel(), value_);
    }
    else
    {
      double npc_acceleration { 0.0 };

      if (not (*api_ptr_).getNPCAccel(trigger_, &npc_acceleration))
      {
        ROS_ERROR_STREAM("Invalid trigger name specified for " << getType() << " condition named " << getName());
        return result_ = false;
      }
      else
      {
        return result_ = compare_(npc_acceleration, value_);
      }
    }
  }
}

}  // namespace condition_plugins

PLUGINLIB_EXPORT_CLASS(condition_plugins::AccelerationCondition, scenario_conditions::ConditionBase)

