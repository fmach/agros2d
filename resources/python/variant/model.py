_empty_svg = '<?xml version="1.0" encoding="UTF-8" standalone="no"?><svg xmlns="http://www.w3.org/2000/svg" version="1.0" width="32" height="32" viewBox="0 0 32 32"></svg>'

import abc
import pickle
import os

class StrictDict(dict):
    """Dictionary for model data.
    Class is inherited from `dict` and implement data type checking.
    """

    def __init__(self, *args, **kwargs):
        self.update(*args, **kwargs)
        self._data_types = dict()

    @property
    def data_types(self):
        """Return declared items."""
        return self._data_types

    def declare(self):
        raise NotImplementedError()

    def fulfilled(self):
        """Check that all declared items (`self.data_types`) are defined.
        :returns: bool -- Return True if all items are defined otherwise return False.
        """

        for key in self._data_types.keys():
            if not key in self.keys(): return False
        return True

    def __setitem__(self, name, value):
        if not name in self._data_types:
            raise KeyError('Item with name "{0}" is not declared!'.format(name))

        data_type = self._data_types[name]
        if ((type(value) != data_type) and
            (not (data_type == float and type(value) == int))):
            raise TypeError('Data type of value do not correspond with defined data type!')

        self.update({name : value})

class Parameters(StrictDict):
    """Dictionary for model input parameters.
    Class is inherited from :class:`StrictDict` and implement default values of declared items.
    """

    def declare(self, name, data_type, default=None):
        """Declare new model input parameter.
        :param name: Name of new parameter.
        :type name: str
        :param data_type: Data type of parameter.
        :type data_type: type
        :param default: Default value for parameter (default is None).
        :type default: Data type depands on `data_type`.
        """

        if default:
            if ((type(default) != data_type) and
                (not (data_type == float and type(default) == int))):
                raise TypeError('Value data type do not correspond with defined data type!')
            self.update({name : default})

        self._data_types[name] = data_type

class Variables(StrictDict):
    """Dictionary for model output variables.
    Class is inherited from :class:`StrictDict`.
    """

    def declare(self, name, data_type):
        """Declare new model output variable.
        :param name: Name of new variable.
        :type name: str
        :param data_type: Data type of variable.
        :type data_type: type
        """
        self._data_types[name] = data_type

class ModelData(object):
    """Class collected all model data.
    Class attribute `parameters` keeps input parameters, attribute `variables` output variables of model.
    Both attributes must be instance of class inherited from :class:`StrictDict`.
    Attribute `info` serve as repository for all other data.
    """

    def __init__(self, model_class):
        self.parameters = Parameters()
        self.variables = Variables()

        self.info = {'_geometry' : _empty_svg}
        self.model_class = model_class

class ModelBase(metaclass=abc.ABCMeta):
    """Abstract base class for description of the model."""

    def __init__(self):
        self._data = ModelData(type(self))
        self.declare()

    @property
    def data(self):
        """Return model data."""
        return self._data

    @data.setter
    def data(self, value):
        raise RuntimeError('Object cannot be overwrite!')

    @property
    def parameters(self):
        """Return model input parameters."""
        return self._data.parameters

    @parameters.setter
    def parameters(self, value):
        raise RuntimeError('Object cannot be overwrite!')

    @property
    def variables(self):
        """Return model output variables."""
        return self._data.variables

    @variables.setter
    def variables(self, value):
        raise RuntimeError('Object cannot be overwrite!')

    @property
    def info(self):
        """Optional info dictionary."""
        return self._data.info

    @info.setter
    def info(self, value):
        self._data.info = value

    @property
    def solved(self):
        """Solution state of the model.
        :returns: bool -- Return True if all declared variables are defined otherwise return False.
        """

        return self._data.variables.fulfilled()

    @solved.setter
    def solved(self, value):
        """Clear all output variables."""
        if not value: self._data.variables.clear()

    @abc.abstractmethod
    def declare(self):
        """Abstract method for declaration of model input parameters and ouput variables.
        Method is called in class constructor and must be redefine in inhereted class.
        For declaration of new parameter use :func:`self.parameters.declare` and for variable :func:`self.variables.declare`.
        """
        pass

    def create(self):
        """Method for creation of model from input parameters.
        Method is called by other OptiLab modules and should be redefine in inhereted class
        """
        pass

    @abc.abstractmethod
    def solve(self):
        """Abstract method for solution of model and calculation of variables.
        Method must be redefine in inhereted class.
        """
        pass

    def load(self, file_name):
        """Unpickle and load model data from binary file (marshalling of model object).
        :param file_name: The file name of binary file with pickled data.
        :type file_name: str
        """

        with open(file_name, 'rb') as infile:
            self._data = pickle.load(infile)

    def save(self, file_name):
        """Pickle and save model data to binary file (serialization of model object).
        :param file_name: The file name of binary file for pickled data.
        :type file_name: str
        """

        directory = os.path.dirname(file_name)
        if not os.path.isdir(directory):
            os.makedirs(directory)

        with open(file_name, 'wb') as outfile:
            pickle.dump(self._data, outfile, 0) #pickle.HIGHEST_PROTOCOL

    def serialize(self):
        """Serialize model data to string.
        :returns: str -- Return string with serialized instance of :class:`ModelData` (:func:`self.data`).
        """

        return pickle.dumps(self._data, 0) #pickle.HIGHEST_PROTOCOL

    def deserialize(self, data):
        """Deserialize and set model data from string.
        :param data: String for deserializaion.
        :type data: str
        """

        self._data = pickle.loads(data)

if __name__ == '__main__':
    import pythonlab

    class Model(ModelBase):
        def declare(self):
            self.parameters.declare('p', float, default = 1.0)
            self.variables.declare('v', float)

        def solve(self):
            pass

    model = Model()
    model.parameters['p'] = 2.0
    model.variables['v'] = 2.0

    file_name = '{0}/model.pickle'.format(pythonlab.tempname())
    model.save(file_name)
    model.load(file_name)