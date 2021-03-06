import os
import subprocess
import re

class ModelDict:
    def __init__(self):
        self._dict = dict()
        self._directory = os.getcwd() + '/models/'

    @property
    def dict(self):
        """Return models dictionary."""
        return self._dict

    @property
    def models(self):
        """Return list of models in dictionary."""
        return list(self._dict.values())

    @property
    def solved_models(self):
        """Return list of solved models in dictionary."""
        models = []
        for model in list(self._dict.values()):
            if model.solved: models.append(model)
        return models

    @property
    def directory(self):
        """Current working directory."""
        return self._directory

    @directory.setter
    def directory(self, value):
        """
        if len(self.models):
            raise RuntimeError('Current working directory can not be changed (dictionary is not empty).')
        """

        if not os.path.isdir(value):
            try:
                os.makedirs(value)
            except OSError:
                raise IOError('Directory "{0}" can not be created.'.format(value))

        self._directory = os.path.abspath(value)

    def _model_file_name(self, name):
        return '{0}/{1}.pickle'.format(self.directory, name)

    def add_model(self, model, name=''):
        """Add new model to dictionary.

        add_model(model, name='')

        Keyword arguments:
        model -- model object inherited from ModelBase class
        name -- name of model used as dictionary key and file name (default is automatic model name)
        """

        if not name:
            if hasattr(self, '_name_index'):
                self._name_index += 1
            else:
                self._name_index = 0
                """
                files = self.find_files('{0}/model_.*.pickle'.format(self.directory))
                for file_name in files:
                    name, extension = os.path.basename(file_name).split(".")
                    index = int(name.split("_")[1])
                    if (index >= self._name_index):
                        self._name_index = index + 1
                """

            name = 'model_{0:06d}'.format(self._name_index)

        """
        if os.path.isfile(self._model_file_name(name)):
            raise KeyError('Model file "{0}" already exist.'.format(self._model_file_name(name)))
        """

        self._dict[name] = model

    def delete_model(self, name):
        """Delete existing model in dictionary.

        delete_model(name)

        Keyword arguments:
        name -- name of model used as dictionary key
        """
        if name in self._dict.keys():
            del self._dict[name]

    def find_model_by_parameters(self, parameters):
        """Find and return model in dictionary by parameters.

        find_model_by_parameters(parameters)

        Keyword arguments:
        parameters -- list of model parameters
        """

        for name, model in self._dict.items():
            if (model.parameters == parameters): return model

    def find_files(self, mask):
        """Find and return model files in directory.

        find_files(mask)

        Keyword arguments:
        mask -- regular expression for directory or files
        """

        files = []
        if os.path.isdir(mask):
            files = os.listdir(mask)
        else:
            for file_name in os.listdir(os.path.dirname(mask)):
                if bool(re.match(r'^{0}$'.format(os.path.basename(mask)), file_name)): files.append(file_name)

        return files

    def load(self, model_class, mask=''):
        """Load models from directory to dictionary.

        load(model_class, mask='')

        Keyword arguments:
        model_class -- class inherited from ModelBase class
        mask -- regular expression for directory or files
        """

        if not mask:
            mask = self.directory
        elif not os.path.dirname(mask):
            mask = '{0}/{1}'.format(self.directory, mask)
        elif (os.path.abspath(os.path.dirname(mask)) != self.directory and
              len(self.models)):
            raise RuntimeError('Mask do not match with current working directory.')

        files = self.find_files(mask)
        for file_name in files:
            model = model_class()
            model.load('{0}/{1}'.format(self.directory, file_name))
            name, extension = os.path.basename(file_name).split(".")
            self._dict[name] = model

    def save(self):
        """Save models from dictionary to current working directory."""
        for name, model in self._dict.items():
            model.save(self._model_file_name(name))

    def solve(self, mask='', recalculate=False, save=True):
        """Solve model in directory.

        solve(mask='', recalculate=False)

        Keyword arguments:
        mask -- regular expression for model keys (in default solve all models)
        recalculate -- recalculate solved models (default is False)
        """

        models = {}
        if not mask:
            models = self._dict
        else:
            for name, model in self._dict.items():
                if bool(re.match(r'^{0}$'.format(mask), name)): models[name] = model

        for name, model in models.items():
            solve_model = recalculate or not model.solved
            if not solve_model: continue

            model.create()
            model.solve()
            model.process()

            if save: model.save(self._model_file_name(name))

    def update(self):
        """Update dictionary from models in current working dictionary."""
        for name in list(self._dict.keys()):
            self._dict[name].load(self._model_file_name(name))

    def clear(self):
        """Clear dictionary."""
        self._dict.clear()

class ModelDictExternal(ModelDict):
    def __init__(self):
        ModelDict.__init__(self)
        self.solver = "agros2d_solver"
        self.solver_parameters = ['-l', '-c']
        self._output = []

    @property
    def output(self):
        """Output of external solver."""
        return self._output

    def solve(self, mask='', recalculate=False):
        """Solve model in directory.

        solve(mask='', recalculate=False)

        Keyword arguments:
        mask -- regular expression for model keys (in default solve all models)
        recalculate -- recalculate solved models (default is False)
        """

        models = {}
        if not mask:
            models = self._dict
        else:
            for name, model in self._dict.items():
                if bool(re.match(r'^{0}$'.format(mask), name)): models[name] = model

        for name, model in models.items():
            solve_model = recalculate or not model.solved
            if not solve_model: continue

            code = "import sys; sys.path.insert(0, '{0}');".format(os.path.dirname(self.directory))
            code += "from problem import {0}; model = {0}();".format(type(model).__name__)
            code += "model.load('{0}/{1}.pickle');".format(self.directory, name)
            code += "model.create(); model.solve(); model.process();"
            code += "model.save('{0}/{1}.pickle')".format(self.directory, name)
            command = ['{0}'.format(self.solver)] + self.solver_parameters + ['{0}'.format(code)]

            process = subprocess.Popen(command, stdout=subprocess.PIPE)
            self._output.append(process.communicate())
            model.load('{0}/{1}.pickle'.format(self.directory, name))
