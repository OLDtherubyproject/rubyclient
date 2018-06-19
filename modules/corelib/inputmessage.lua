function InputMessage:getData()
  local dataType = self:getByte()
  if dataType == NetworkMessageTypes.Boolean then
    return numbertoboolean(self:getByte())
  elseif dataType == NetworkMessageTypes.U8 then
    return self:getByte()
  elseif dataType == NetworkMessageTypes.U16 then
    return self:getU16()
  elseif dataType == NetworkMessageTypes.U32 then
    return self:getU32()
  elseif dataType == NetworkMessageTypes.U64 then
    return self:getU64()
  elseif dataType == NetworkMessageTypes.NumberString then
    return tonumber(self:getString())
  elseif dataType == NetworkMessageTypes.String then
    return self:getString()
  elseif dataType == NetworkMessageTypes.Table then
    return self:getTable()
  else
    perror('Unknown data type ' .. dataType)
  end
  return nil
end

function InputMessage:getTable()
  local ret = {}
  local size = self:getU16()
  for i=1,size do
    local index = self:getData()
    local value = self:getData()
    ret[index] = value
  end
  return ret
end

function InputMessage:getColor()
  local color = {}
  color.r = self:getByte()
  color.g = self:getByte()
  color.b = self:getByte()
  color.a = self:getByte()
  return color
end

function InputMessage:getPosition()
  local position = {}
  position.x = self:getU16()
  position.y = self:getU16()
  position.z = self:getByte()
  return position
end
